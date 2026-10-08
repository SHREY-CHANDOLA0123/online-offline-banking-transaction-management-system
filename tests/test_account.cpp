#include <iostream>
#include "../src/models/Account.h"
using namespace std;

int main()
{   Account account("ACC1001", "Test User", 100000);
    cout << "Account ID: " << account.getAccountId() << '\n';
    cout << "Holder: " << account.getHolderName() << '\n';
    cout << "Initial Balance: " << account.getBalance() << " paise\n";

    // Credit ₹500
    if(account.credit(50000))
        {cout << "Credit successful\n";
        }

    // Debit ₹200
    if(account.debit(20000))
        {cout << "Debit successful\n";
        }

    // Try to debit more than available balance
    if(!(account.debit(1000000)))
        {cout << "Large debit rejected\n";
        }

    cout << "Final Balance: " << account.getBalance() << " paise\n"; 

    return 0;
}