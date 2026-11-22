OOBTMS — LAPTOP / PC PACKAGE

This package runs the existing C++ bank server and admin dashboard. It includes the original C++ account/transaction classes and desktop customer site. The Termux phone client is supplied separately.

WINDOWS QUICK START
1. Extract this folder.
2. Install MinGW-w64 (g++) and ensure g++ is on PATH.
3. Double-click start.bat.
4. Admin dashboard: http://localhost:8000
   Original desktop customer site: http://localhost:8001
5. Keep the bank_server window running during the phone demo.

DEMO DATA
- Customer A: ACC1001, initial balance Rs 2,000, PIN 1234.
- Customer B: ACC1002.
- Customer C: ACC1003.
- The included transaction history starts empty.
- The offline demo allows a Rs 3,000 queued payment from Customer A; after sync, the simulated balance becomes -Rs 1,000.

PHONE CONNECTION
- Laptop and phone must be connected to the same Wi-Fi network when synchronizing.
- Find the laptop Wi-Fi IPv4 address with `ipconfig` (look under the active Wi-Fi adapter).
- Allow inbound TCP port 8000 through Windows Firewall on Private networks.
- Start the phone client using the laptop IPv4 address, NOT localhost. Example: 192.168.1.5

The server binds to all network interfaces (0.0.0.0) on port 8000 so the phone can reach it over the LAN. The admin page on the laptop can still use localhost.

This is a classroom simulation only, not a secure banking system. Do not expose it to the public internet or use real money.
