// The bank's server: business rules (using your classes) + HTTP routes for the customer app and the control room.
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "../include/managers/AccountManager.h"
#include "../include/managers/TransactionManager.h"
#include "../include/managers/AuthenticationManager.h"
#include "../include/network/NetworkManager.h"
#include "../include/security/RiskManager.h"
#include "../include/storage/FileManager.h"
#include "../include/utils/InputValidation.h"
#include "../include/utils/Logger.h"
using namespace std;

static AccountManager accounts; static TransactionManager history; static AuthenticationManager auth; static NetworkManager net;
struct Device { string id, acc, tok; int q; long long qa; time_t seen; };
static vector<Device> devices;

static string money(long long p) { char b[40]; snprintf(b, sizeof b, "%lld.%02lld", p / 100, p % 100); return b; }
static string nameOf(const string& id) { const Account* a = accounts.findAccountById(id); return a ? a->getHolderName() : id; }
static string query(const string& path, const string& key) {
    size_t q = path.find('?'); if (q == string::npos) return "";
    for (auto& kv : InputValidation::split(path.substr(q + 1), '&')) if (kv.rfind(key + "=", 0) == 0) return InputValidation::sanitize(kv.substr(key.size() + 1));
    return "";
}
static void seedDemo() {
    accounts.clear(); history.clear(); srand((unsigned)time(nullptr));
    const char* names[] = {"Customer A", "Customer B", "Customer C"};
    for (int i = 0; i < 3; i++) {
        string id = "ACC100" + to_string(i + 1); long long rupees = (i == 0) ? 2000 : 2000 + (rand() % 181) * 100;   // Customer A starts at Rs 2,000 for the offline demo
        accounts.addAccount(Account(id, names[i], rupees * 100)); auth.ensureUser(id);
    }
    FileManager::save(accounts, history); Logger::log("Demo data reset: customers A, B, C created with random balances");
}

// Settle one payment. PENDING -> SUCCESSFUL or FAILED (Transaction's own state rules). Same ID twice returns the first result.
static const Transaction* settle(const string& id, const string& from, const string& to, long long amt, const string& created, bool offline) {
    if (Transaction* old = history.findTransactionById(id)) return old;
    Transaction t(id, from, to, amt, created);
    Account *s = accounts.findAccountById(from), *r = accounts.findAccountById(to); string bad;
    if (id.empty() || !InputValidation::isValidAmount(amt)) bad = "Invalid request";
    else if (!s || !r || from == to) bad = "Invalid accounts";
    else if (r->getStatus() == AccountStatus::BLOCKED) bad = "Receiver account blocked";
    else if (offline && !RiskManager::canSettleOffline(*s, amt)) bad = "Over offline limit";
    else if (!offline && !RiskManager::canSettleOnline(*s, amt)) bad = "Insufficient balance";
    if (bad.empty()) { offline ? s->debitOffline(amt) : s->debit(amt); r->credit(amt); t.updateStatus(TransactionStatus::SUCCESSFUL); }
    else t.updateStatus(TransactionStatus::FAILED);
    t.setSettlement(offline ? "OFFLINE" : "ONLINE", bad, Logger::timeNow());
    if (!history.addTransaction(t)) return history.findTransactionById(id);
    Logger::log(id + (offline ? " synced: " : " online: ") + nameOf(from) + " to " + nameOf(to) + " Rs " + money(amt) + " - " + FileManager::statusName(t.getStatus()) + (bad.empty() ? "" : " (" + bad + ")"));
    return &history.getAllTransactions().back();
}
static string txJson(const Transaction& t) {
    return "{\"id\":\"" + t.getTransactionId() + "\",\"from\":\"" + t.getSenderAccountId() + "\",\"to\":\"" + t.getReceiverAccountId() + "\",\"amount\":" + to_string(t.getAmount()) +
           ",\"mode\":\"" + t.getMode() + "\",\"createdAt\":\"" + t.getCreatedAt() + "\",\"settledAt\":\"" + t.getSettledAt() + "\",\"status\":\"" + FileManager::statusName(t.getStatus()) + "\",\"reason\":\"" + t.getReason() + "\"}";
}
static string stateJson() {
    string s = "{\"offlineLimit\":" + to_string(RiskManager::OFFLINE_LIMIT) + ",\"accounts\":{"; bool first = true;
    for (const Account* a : accounts.getAllAccounts()) { s += (first ? "" : ",") + string("\"") + a->getAccountId() + "\":{\"name\":\"" + a->getHolderName() + "\",\"balance\":" + to_string(a->getBalance()) + ",\"status\":\"" + (a->getStatus() == AccountStatus::ACTIVE ? "ACTIVE" : "BLOCKED") + "\"}"; first = false; }
    s += "},\"transactions\":[";
    for (size_t i = 0; i < history.getAllTransactions().size(); i++) s += (i ? "," : "") + txJson(history.getAllTransactions()[i]);
    return s + "]}";
}
static string parseTx(const string& line, bool offline) {   // line format: id|from|to|amountPaise|createdAt
    auto f = InputValidation::split(line, '|'); if (f.size() < 5) return "";
    long long a = 0; try { a = stoll(f[3]); } catch (...) {}
    return txJson(*settle(InputValidation::sanitize(f[0]), InputValidation::sanitize(f[1]), InputValidation::sanitize(f[2]), a, InputValidation::sanitize(f[4]), offline));
}
static void serve(const string& file) {
    ifstream f(file, ios::binary); if (!f) return net.respond(404, "text/plain", "Missing " + file + " - start the server from the project folder");
    stringstream ss; ss << f.rdbuf(); net.respond(200, "text/html; charset=utf-8", ss.str());
}
static void handle(const HttpRequest& r) {
    const string &m = r.method, &p = r.path, &body = r.body;
    if (m == "GET" && p.rfind("/api/state", 0) == 0) {
        string d = query(p, "dev");
        if (!d.empty()) {   // heartbeat: the device reports itself and its offline queue
            time_t n = time(nullptr); Device* v = nullptr; for (auto& x : devices) if (x.id == d) v = &x;
            int q = 0; long long qa = 0; try { q = stoi("0" + query(p, "q")); qa = stoll("0" + query(p, "qa")); } catch (...) {}
            string acc = query(p, "acc");
            if (!v) { devices.push_back({d, acc, "", 0, 0, n}); v = &devices.back(); Logger::log(d + " connected"); }
            else if (n - v->seen > 3) Logger::log(d + " is back online with " + to_string(q) + " queued payment(s)" + (q ? " worth Rs " + money(qa) : ""));
            v->acc = acc; v->q = q; v->qa = qa; v->tok = query(p, "tok"); v->seen = n;
        }
        return net.respond(200, "application/json", stateJson());
    }
    if (m == "GET" && p.rfind("/api/admin", 0) == 0) {
        time_t n = time(nullptr); string o = "{\"state\":" + stateJson() + ",\"devices\":[";
        for (size_t i = 0; i < devices.size(); i++) { auto& d = devices[i]; o += (i ? "," : "") + string("{\"id\":\"") + d.id + "\",\"acc\":\"" + d.acc + "\",\"q\":" + to_string(d.q) + ",\"qa\":" + to_string(d.qa) + ",\"tok\":\"" + d.tok + "\",\"ago\":" + to_string((long long)(n - d.seen)) + "}"; }
        o += "],\"events\":["; auto& ev = Logger::recent(); for (size_t i = 0; i < ev.size(); i++) o += (i ? "," : "") + string("\"") + ev[i] + "\"";
        return net.respond(200, "application/json", o + "]}");
    }
    if (m == "POST" && p == "/api/login") {
        auto f = InputValidation::split(body, '|'); bool ok = f.size() == 2 && auth.login(InputValidation::sanitize(f[0]), f[1]);
        if (ok) Logger::log(nameOf(f[0]) + " signed in");
        return net.respond(200, "application/json", ok ? "{\"ok\":true}" : "{\"ok\":false}");
    }
    if (m == "POST" && p == "/api/transfer") { string t = parseTx(body, false); FileManager::save(accounts, history); return net.respond(200, "application/json", "{\"tx\":" + (t.empty() ? "{}" : t) + ",\"state\":" + stateJson() + "}"); }
    if (m == "POST" && p == "/api/sync") {
        string out = "{\"results\":["; bool first = true;
        for (auto& line : InputValidation::split(body, '\n')) { string t = parseTx(line, true); if (t.empty()) continue; out += (first ? "" : ",") + t; first = false; }
        FileManager::save(accounts, history); return net.respond(200, "application/json", out + "],\"state\":" + stateJson() + "}");
    }
    if (m == "POST" && p == "/api/reset") { seedDemo(); return net.respond(200, "application/json", stateJson()); }
    if (m == "GET") return serve("web/admin.html");
    net.respond(404, "text/plain", "Not found");
}
int runServer() {
    auth.load(); FileManager::load(accounts, history);
    if (accounts.isEmpty()) seedDemo();
    for (const Account* a : accounts.getAllAccounts()) auth.ensureUser(a->getAccountId());
    if (!net.start(8000)) { cerr << "Port 8000 is busy. Close the other server first.\n"; return 1; }
    cout << "OOBTMS BANK SERVER (admin site) running.\n  Admin dashboard: http://localhost:8000\n  Waiting for the customer site on port 8001...\n";
    for (;;) { HttpRequest r; if (net.nextRequest(r)) handle(r); }
}
