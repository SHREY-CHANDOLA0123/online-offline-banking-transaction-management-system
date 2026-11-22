// The CUSTOMER SITE (port 8001): serves the customer app, keeps the offline queue,
// and talks to the BANK SITE (port 8000) over HTTP. If the bank cannot be reached, payments wait in the queue.
#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/network/NetworkManager.h"
#include "../include/security/RiskManager.h"
#include "../include/utils/InputValidation.h"
using namespace std;

struct Queued { string id, from, to; long long amount; string created; };
static NetworkManager net; static vector<Queued> waiting;
static string cache = "{\"offlineLimit\":500000,\"accounts\":{},\"transactions\":[]}";   // last state received from the bank
static bool online = false, cut = false; static time_t lastBeat = 0;
const int BANK_PORT = 8000;
static const char* JSON = "application/json";

static string rupees(long long p) { char b[40]; snprintf(b, sizeof b, "%lld.%02lld", p / 100, p % 100); return b; }
static string queuedJson(const Queued& q) {
    return "{\"id\":\"" + q.id + "\",\"from\":\"" + q.from + "\",\"to\":\"" + q.to + "\",\"amount\":" + to_string(q.amount) + ",\"createdAt\":\"" + q.created + "\",\"settledAt\":\"\",\"mode\":\"OFFLINE\",\"status\":\"PENDING\",\"reason\":\"\"}";
}
static string stateResponse() {
    string q; for (size_t i = 0; i < waiting.size(); i++) q += (i ? "," : "") + queuedJson(waiting[i]);
    return string("{\"online\":") + (online ? "true" : "false") + ",\"cut\":" + (cut ? "true" : "false") + ",\"queue\":[" + q + "],\"state\":" + cache + "}";
}
// Runs about once a second: heartbeat to the bank (it also reports our queue), then settle the queue if the bank answered.
static void background() {
    long long qa = 0; string tok; for (auto& q : waiting) { qa += q.amount; tok += (tok.empty() ? "" : "_") + q.id; }
    string out, path = "/api/state?dev=CUSTOMER-SITE&acc=ALL&q=" + to_string(waiting.size()) + "&qa=" + to_string(qa) + "&tok=" + tok;
    bool ok = !cut && NetworkManager::call(BANK_PORT, "GET", path, "", out);
    if (ok) {
        cache = out;
        if (!waiting.empty()) {
            string body, reply; for (auto& q : waiting) body += q.id + "|" + q.from + "|" + q.to + "|" + to_string(q.amount) + "|" + q.created + "\n";
            if (NetworkManager::call(BANK_PORT, "POST", "/api/sync", body, reply)) waiting.clear();
        }
    }
    online = ok;
}
static void handle(const HttpRequest& r) {
    const string &m = r.method, &p = r.path;
    if (m == "GET" && p.rfind("/api/state", 0) == 0) return net.respond(200, JSON, stateResponse());
    if (m == "POST" && p == "/api/cut") { cut = !cut; if (cut) online = false; return net.respond(200, JSON, stateResponse()); }
    if (m == "POST" && p == "/api/login") {
        string out; if (!online || !NetworkManager::call(BANK_PORT, "POST", "/api/login", r.body, out)) out = "{\"ok\":false,\"offline\":true}";
        return net.respond(200, JSON, out);
    }
    if (m == "POST" && p == "/api/reset") { string out; if (online) NetworkManager::call(BANK_PORT, "POST", "/api/reset", "", out); waiting.clear(); return net.respond(200, JSON, stateResponse()); }
    if (m == "POST" && p == "/api/pay") {
        auto f = InputValidation::split(r.body, '|'); long long amt = 0; if (f.size() >= 5) { try { amt = stoll(f[3]); } catch (...) {} }
        if (f.size() < 5 || !InputValidation::isValidAmount(amt)) return net.respond(200, JSON, "{\"error\":\"Invalid payment\"}");
        Queued q{InputValidation::sanitize(f[0]), InputValidation::sanitize(f[1]), InputValidation::sanitize(f[2]), amt, InputValidation::sanitize(f[4])};
        if (online) { string out; if (NetworkManager::call(BANK_PORT, "POST", "/api/transfer", r.body, out)) return net.respond(200, JSON, out); online = false; }
        long long pending = 0; for (auto& x : waiting) if (x.from == q.from) pending += x.amount;   // bank unreachable: save offline, within the limit
        if (pending + amt > RiskManager::OFFLINE_LIMIT)
            return net.respond(200, JSON, "{\"error\":\"Offline limit reached. You can still send Rs " + rupees(max(0LL, RiskManager::OFFLINE_LIMIT - pending)) + " until you reconnect.\"}");
        waiting.push_back(q); return net.respond(200, JSON, "{\"queued\":true,\"tx\":" + queuedJson(q) + "}");
    }
    if (m == "GET") {
        ifstream f("web/index.html", ios::binary); if (!f) return net.respond(404, "text/plain", "Missing web/index.html - start from the project folder");
        stringstream ss; ss << f.rdbuf(); return net.respond(200, "text/html; charset=utf-8", ss.str());
    }
    net.respond(404, "text/plain", "Not found");
}
int runCustomerSite() {
    if (!net.start(8001)) { cerr << "Port 8001 is busy. Close the other customer site first.\n"; return 1; }
    cout << "OOBTMS CUSTOMER SITE running.\n  Customer app: http://localhost:8001\n  Talks to the bank site on port 8000 (start bank_server too).\n";
    for (;;) {
        HttpRequest r; if (net.nextRequest(r, 1000)) handle(r);
        if (time(nullptr) != lastBeat) { lastBeat = time(nullptr); background(); }
    }
}
