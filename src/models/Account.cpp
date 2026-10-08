#include "Account.h"
using namespace std;

Account::Account(const string &accountId, const string &holderName, long long balance, AccountStatus status)
{   this->accountId = accountId;
    this->holderName = holderName;
    this->balance = balance;
    this->status = status;
}

string Account::getAccountId() const
{   return accountId;
}

string Account::getHolderName() const
{   return holderName;
}

long long Account::getBalance() const
{   return balance;
}

AccountStatus Account::getStatus() const
{   return status;
}

void Account::setStatus(AccountStatus newStatus)
{   status = newStatus;
}

bool Account::credit(long long amount)
{   if(amount <= 0)
        {return false;
        }
    balance += amount;
    return true;
}

bool Account::debit(long long amount)
{   if(amount <= 0)
        {return false;
        }

    if(amount > balance)
        {return false;
        }
    balance -= amount;
    return true;
}