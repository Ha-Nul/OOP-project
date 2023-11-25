#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Bank;
class Account;


/*-------------------------------------Class region-------------------------------------------------------------------------------*/

class ATM {
private:
	static long long atm_count;
	const long long serial_number;
	const long long admin_card_number;
	const long long language_type; // Unilingual = 0, Bilingual = 1
	int language; // English = 1, Korean = 2
	long long _account_number;
	long long number_of_avialable_cash_list[4] = { 100, 100, 100, 100 }; // {number_of_cash1000, number_of_cash5000, number_of_cash10000, number_of_cash50000}
	long long availabe_cash = number_of_avialable_cash_list[0] * 1000 + number_of_avialable_cash_list[1] * 5000 + number_of_avialable_cash_list[2] * 10000 + number_of_avialable_cash_list[3] * 50000;
	long long receive_check;
	const Bank* primary_bank;
	//string file_name;

	long long Menu();
	void Admin();
	void Admin_SAVE_TEXT(long long ID, long long CARDNUM, int TYPE, long long AMOUNT, string ETC);
	void Deposit();
	void Withdraw();
	void Transfer();
	bool Allowed_check(long long check);
	bool Account_Exist(long long account_number);
	Bank* Call_Bank_of_account(long long account_number);
	void Cancel_Button();

protected:
	vector<Bank*> connected_bank_list;

public:
	ATM(Bank* _primary_bank, long long _language_type );
	void Start_ATM();
};

class MultiBank_ATM : public ATM{
private:

public:
	MultiBank_ATM(Bank* _primary_bank, long long _language_type ) : ATM( _primary_bank, _language_type ){};
	void Add_connected_bank_list(Bank* _bank);
};
class SingleBank_ATM : public ATM{
public:
	SingleBank_ATM(Bank* _primary_bank, long long _language_type ) : ATM( _primary_bank, _language_type ){};
};


class Bank {
private:
	const string bank_name;
	vector<ATM*> atm_list;
	vector<Account*> account_list;

public:
	Bank(string _bank_name);

	string Call_bank_name();
	long long Call_balance(long long _account_num); // Use only after executing Exist account and Confirm_password functions. Use _account_num variable we used in Exist_account

	bool Exist_account(long long _account_num);
	bool Confirm_password(long long _account_num, long long _input_password);// Use only after executing Exist account function. Use _account_num variable we used in Exist_account
	bool Input_balance(long long _account_num, long long _fund);//we should use at Deposit and Tranfer_in_Bank function
	bool Output_balance(long long _account_num, long long _fund);//we should use at Withdraw and Tranfer_in_Bank function. retun false when _fund is larger than balance.
	Account* Find_corresponding_account(long long _account_num);//function name is same as the explanation

	void Add_account_list(Account* _account);//This function is executed by Account constructor.
	void Add_atm_list(ATM* _atm);//This function is executed by  ATM::Add_bank_list function.

};

class Account {
private:
	static long long account_count;
	const string user_name;
	const Bank* bank;
	const long long account_number;
	const long long password;
	long long balance; //control only at Constructor, Input_balance and output_balance functions.
	friend bool Bank::Input_balance(long long _account_num, long long _fund); //set freind function. control balance at this function of Bank class
	friend bool Bank::Output_balance(long long _account_num, long long _fund); //set freind function. control balance at this function of Bank class
public:
	Account(string _user_name, Bank* _bank, long long _password, long long _balance);

	string Call_user_name();
	const Bank* Call_bank();
	long long Call_account_number();
	long long Call_password();
	long long Call_balance();
	bool operator+(long long amount);
	bool operator-(long long amount);
};


long long ATM::atm_count = 0;
long long Account::account_count = 0;

vector<Bank*> bank_list; //Global variables



/*-------------------------------------ATM region-------------------------------------------------------------------------------*/
ATM::ATM(Bank* _primary_bank, long long  _language_type)
	: serial_number(atm_count + 100000),
	language_type(_language_type),
	primary_bank(_primary_bank),
	admin_card_number(atm_count + 100000)
{
	connected_bank_list.push_back(_primary_bank);
	_primary_bank->Add_atm_list(this);
	atm_count++;
	//Error message for preparing code demostration.
	if ((_language_type != 0) && (_language_type != 1)) {
		cout << "ERROR::Please input language_type either 0 or 1" << endl;
	}
}

void ATM::Start_ATM() {
	/*=================== Enter the account number ===================*/
	cout << "Please enter your card(account) number" << endl;
	cin >> _account_number;
	/*================================================================*/

	/*==================== Select the Language ====================*/
	// if ATM is bilingual
	if (language_type == 1) {
		cout << "Please select the language" << endl;
		cout << "1. English" << endl;
		cout << "2. 한국어" << endl;
		cin >> language;
	}
	// if ATM is unilingual
	else {
		language = 1; // Default is English
	}
	/*=============================================================*/

	if (_account_number == admin_card_number) {
		Admin();
		return;
	}

	while (1) {
		try{ //when cancel is pushed, the throw instance instancely called.
			long long menu_type = Menu();

			/*==Deposit==*/
			if (menu_type == 1) {
				Deposit();
			}
			/*==Withdrawal==*/
			else if (menu_type == 2) {
				Withdraw();
			}
			/*==Transfer==*/
			else if (menu_type == 3) {
				Transfer();
			}
			/*==Cancel==*/
			else if (menu_type < 0){//Cancle is called, when the input parameter is negative.
				throw 1;
			}
			Admin_SAVE_TEXT(0 , _account_number, menu_type, 0, "");//Someone is void value <- delete this when this problem is solved.
		}
		catch(int _void){
			Cancel_Button();
			continue;
		}
	}
}

void ATM::Admin_SAVE_TEXT(long long _ID, long long _CARD, int _TYPE, long long _AMOUNT, string ETC) {

	string SAVE_MENU_NAME;

	if (_TYPE == 1) {
		SAVE_MENU_NAME = "Deposit";
	}
	else if (_TYPE == 2) {
		SAVE_MENU_NAME = "Withdraw";
	}
	else if (_TYPE == 3) {
		SAVE_MENU_NAME = "Transfer";
	}
	else if (_TYPE == 4) {
		SAVE_MENU_NAME = "CANCEL selected";
		ETC = "CANCELED";
	}

	ofstream out("TRANSACTION_HISTORY.txt", ios::app);
	string s;

	if (out.is_open())
	{
		out << _ID << "\t" << _CARD << "\t" << SAVE_MENU_NAME << "\t" << _AMOUNT << "\t" << ETC << "\n";
	}
}

bool ATM::Allowed_check(long long check) {
	bool result = check >= 100000;
	if (result == false) {
		if (language == 1) {
			cout << "The amount of check is not allowed" << endl;
			cout << "Any amout over KRW 100000 check is allowed" << endl;
		}
		else {
			cout << "허용되지 않은 수표의 금액입니다" << endl;
			cout << "100000 원 이상의 수표만 허용됩니다" << endl;
		}
	}
	return result;
}

bool ATM::Account_Exist(long long account_number) {
	Bank* bank;
	for (int i = 0; i < connected_bank_list.size(); i++) {
		bank = connected_bank_list[i];
		if (bank->Exist_account(account_number)) {
			return true;
		}
	}
	return false;
}

Bank* ATM::Call_Bank_of_account(long long account_number) {
	Bank* bank;
	for (int i = 0; i < connected_bank_list.size(); i++) {
		bank = connected_bank_list[i];
		if (bank->Exist_account(account_number)) {
			return bank;
		}
	}
	return nullptr;
}


long long ATM::Menu() {
	return 0;
}
void ATM::Admin() {
	return;
}
void ATM::Deposit() {
	return ;//when successfully ended
}
void ATM::Withdraw() {
	return ;//when successfully ended
}

void ATM::Transfer() {
	/*===================== Set the Source Bank =====================*/
	Bank* source_bank = Call_Bank_of_account(_account_number);
	/*===============================================================*/

	/*==================== Select the Transfer Type ====================*/
	int transfer_type; // Cash transfer = 1, Account transfer = 2
	while (1) {
		if (language == 1) {
			cout << "Please select the transfer type" << endl;
			cout << "1. Cash transfer" << endl;
			cout << "2. Account transfer" << endl;
		}
		else {
			cout << "이체 형식을 선택해주세요" << endl;
			cout << "1. 현금 이체" << endl;
			cout << "2. 계좌 이체" << endl;
		}
		cin >> transfer_type;
		if (transfer_type == 1) {
			if (language == 1) {
				cout << "Cash transfer was selected" << endl;
			}
			else {
				cout << "현금 이체가 선택되었습니다" << endl;
			}
			break;
		}
		else if (transfer_type == 2) {

			if (language == 1) {
				cout << "Account transfer is selected" << endl;
				cout << "The withdrawal account is " << source_bank->Call_bank_name() << " " << _account_number << endl;
			}
			else {
				cout << "계좌 이체가 선택되었습니다" << endl;
				cout << "출금 계좌는 " << source_bank->Call_bank_name() << " " << _account_number << " 입니다" << endl;
			}
			break;
		}
		else if (transfer_type < 0){//Cancle is called
			throw 1;
		}
		else {
			if (language == 1) {
				cout << "Please enter the number 1 or 2" << endl;
			}
			else {
				cout << "숫자 1 또는 2를 입력해주세요" << endl;
			}
			continue;
		}
	}
	/*==================================================================*/

	/*============= Enter the Destination Account Number ==============*/
	long long  destination_account_number;
	while (1) {
		if (language == 1) {
			cout << "Please enter the destination account number" << endl;
		}
		else {
			cout << "입금 계좌 번호를 입력해주세요" << endl;
		}
		cin >> destination_account_number;
		if(destination_account_number < 0){//Cancle is called
			throw 1;
		}
		else if (Account_Exist(destination_account_number)) {
			if (language == 1) {
				cout << "The destination account number is " << destination_account_number << endl;
			}
			else {
				cout << "입금 계좌 번호는 " << destination_account_number << " 입니다" << endl;
			}
			break;
		}
		else {
			if (language == 1) {
				cout << "The destination account number is not exist" << endl;
			}
			else {
				cout << "입금 계좌 번호가 존재하지 않습니다" << endl;
			}
			continue;
		}
	}
	Bank* destination_bank = Call_Bank_of_account(destination_account_number);
	/*=================================================================*/

	/*==================================== Transfer ====================================*/
	// if Cash transfer
	if (transfer_type == 1) {

		/*=================== Recieve Cash Transfer Payment ===================*/
		long long number_of_cash_list[4]; // {number_of_cash_1000, number_of_cash_5000, number_of_cash_10000, number_of_cash_50000}
		int cash_list[4] = { 1000, 5000, 10000, 50000 };
		long long pay = 0;
		long long cash_transfer_fee = 5000;
		while (1) {
			if (language == 1) {
				cout << "Please pay the amount that you want to cash transfer plus cash transfer fee KRW " << cash_transfer_fee << endl;
			}
			else {
				cout << "현금 이체하실 금액에 현금 이체 수수료 " << cash_transfer_fee << " 원을 더하여 지불해주세요" << endl;
			}
			for (int i; i < 4; i++) {
				if (language == 1) {
					cout << "Please enter the number of KRW " << cash_list[i] << " to be paid" << endl;
				}
				else {
					cout << "지불하실" << cash_list[i] << "원의 개수를 입력해주세요" << endl;
				}
				cin >> number_of_cash_list[i];
			}
			for (int i; i < 4; i++) {
				pay += cash_list[i] * number_of_cash_list[i];
			}
			if(pay < 0){//Cancle is called
				throw 1;
			}
			else if (pay <= cash_transfer_fee) {
				if (language == 1) {
					cout << "Your payment is less than or equal to the cash transfer fee" << endl;
				}
				else {
					cout << "지불하신 금액이 현금 이체 수수료보다 작거나 같습니다" << endl;
				}
				continue;
			}
			else {
				if (language == 1) {
					cout << "The total amount that you paid is KRW " << pay << endl;
					cout << "The amount of cash transfer: KRW " << pay - cash_transfer_fee << endl;
					cout << "Cash transfer fee: KRW " << cash_transfer_fee << endl;
				}
				else {
					cout << "지불하신 총 금액은 " << pay << " 원입니다" << endl;
					cout << "현금 이체 금액: " << pay << " 원" << endl;
					cout << "현금 이체 수수료: " << cash_transfer_fee << " 원" << endl;
				}
				availabe_cash += pay;
				break;
			}
		}
		/*=====================================================================*/

		/*============== Increase Balance of Destination Account ==============*/
		destination_bank->Input_balance(destination_account_number, pay - cash_transfer_fee);
		/*=====================================================================*/

		/*============================ End Session ============================*/
		if (language == 1) {
			cout << "The cash transfer of KRW " << pay - cash_transfer_fee << " to " << destination_bank->Call_bank_name() << " " << destination_account_number << " was performed successfully" << endl;
		}
		else {
			cout << pay - cash_transfer_fee << " 원이 " << destination_bank->Call_bank_name() << " " << destination_account_number << " 으로 현금 이체 되었습니다" << endl;
		}
		return;
		/*=====================================================================*/
	}

	// if Account transfer
	if (transfer_type == 2) {

		/*=================== Determine Account Transfer Fee ===================*/
		long long account_transfer_fee;
		if (source_bank == primary_bank) {
			if (destination_bank == primary_bank) {
				account_transfer_fee = 2000;
			}
			else {
				account_transfer_fee = 3000;
			}
		}
		if (source_bank != primary_bank) {
			if (destination_bank == primary_bank) {
				account_transfer_fee = 3000;
			}
			else {
				account_transfer_fee = 4000;
			}
		}
		/*======================================================================*/

		/*================== Enter the Amount of Account Transfer ==================*/
		long long amount_transfer;
		while (1) {
			if (language == 1) {
				cout << "The balance of source account: KRW " << source_bank->Call_balance(_account_number) << endl;
				cout << "In the case of account transfer, the account transfer fee KRW " << account_transfer_fee << " will be paid from the source account" << endl;
				cout << "Please enter the amount of transfer" << endl;
			}
			else {
				cout << "출금 계좌의 잔액: " << source_bank->Call_balance(_account_number) << " 원" << endl;
				cout << "계좌 이체의 경우 계좌 이체 수수료 " << account_transfer_fee << " 원이 출금 계좌에서 추가로 지불됩니다" << endl;
				cout << "송금하실 금액을 입력해주세요" << endl;
			}
			cin >> amount_transfer;
			if (amount_transfer < 0){//Cancle is called
				throw 1;
			}
			else if (source_bank->Call_balance(_account_number) < amount_transfer + account_transfer_fee) {
				if (language == 1) {
					cout << "The balance of source account is not sufficient" << endl;
				}
				else {
					cout << "출금 계좌의 잔액이 부족합니다" << endl;
				}
				continue;
			}
			else {
				if (language == 1) {
					cout << "The amount of account transfer: KRW " << amount_transfer << endl;
					cout << "Account transfer fee: KRW " << account_transfer_fee << endl;
				}
				else {
					cout << "송금 금액: " << amount_transfer << " 원" << endl;
					cout << "계좌 이체 수수료: " << account_transfer_fee << " 원" << endl;
				}
				break;
			}
		}
		/*==========================================================================*/

		/*=================== Decrease and Increase Balance ===================*/
		source_bank->Output_balance(_account_number, amount_transfer + account_transfer_fee);
		destination_bank->Input_balance(destination_account_number, amount_transfer);
		/*=====================================================================*/

		/*============================ End Session ============================*/
		if (language == 1) {
			cout << "The account transfer of KRW " << amount_transfer << " from " << source_bank->Call_bank_name() << " " << _account_number << " to " << destination_bank->Call_bank_name() << " " << destination_account_number << " was performed successfully" << endl;
		}
		else {
			cout << amount_transfer << " 원이 " << source_bank->Call_bank_name() << " " << _account_number << " 에서 " << destination_bank->Call_bank_name() << " " << destination_account_number << " 으로 계좌 이체 되었습니다" << endl;
		}
		return;
		/*=====================================================================*/

	}
	/*==================================================================================*/

	return;
}

void ATM::Cancel_Button(){
	if(language == 1){
		cout << "The cancel button is pressed. Exit the current session." << endl;
	}
	else{
		cout << "취소 버튼을 눌렀습니다. 진행중인 세션을 종료합니다." << endl;
	}
}

/*------------------------------MultiBank_ATM region-----------------------------------------------------------------------------*/


void MultiBank_ATM::Add_connected_bank_list(Bank* _bank) {
	connected_bank_list.push_back(_bank);
	_bank->Add_atm_list(this);
}


/*-------------------------------------Bank region-------------------------------------------------------------------------------*/


Bank::Bank(string _bank_name) : bank_name(_bank_name) {
	bank_list.push_back(this);
}


string Bank::Call_bank_name() {
	return bank_name;
}
long long Bank::Call_balance(long long _account_num) {
	return Find_corresponding_account(_account_num)->Call_balance();
}


bool Bank::Exist_account(long long _account_num) {
	for (int i = 0; i < account_list.size(); i++) {
		if (account_list[i]->Call_account_number() == _account_num) {
			return true;
		}
	}
	return false;
}
bool Bank::Confirm_password(long long _account_num, long long _input_password) {
	if (Find_corresponding_account(_account_num)->Call_password() == _input_password) {
		return true;
	}
	else {
		return false;
	}
}
bool Bank::Input_balance(long long _account_num, long long _fund) {
	return Find_corresponding_account(_account_num) + _fund;
}
bool Bank::Output_balance(long long _account_num, long long _fund) {
	return Find_corresponding_account(_account_num) - _fund;
}

Account* Bank::Find_corresponding_account(long long _account_num) {
	for (int i = 0; i < account_list.size(); i++) {
		if (account_list[i]->Call_account_number() == _account_num) {//Assume that there must exist corresponding account
			return account_list[i];
		}
	}
	return nullptr;
}


void Bank::Add_account_list(Account* _account) {
	account_list.push_back(_account);
}
void Bank::Add_atm_list(ATM* _atm) {
	atm_list.push_back(_atm);
}


/*-------------------------------------Account region-------------------------------------------------------------------------------*/





Account::Account(string _user_name, Bank* _bank, long long _password, long long _balance)
	: user_name(_user_name),
	bank(_bank),
	password(_password),
	balance(_balance),
	account_number(100000000000 + account_count)
{
	account_count++;
	_bank->Add_account_list(this);
}

string Account::Call_user_name() {
	return user_name;
}
const Bank* Account::Call_bank() {
	return bank;
}
long long Account::Call_account_number() {
	return account_number;
}
long long Account::Call_password() {
	return password;
}
long long Account::Call_balance() {
	return balance;
}
bool Account::operator+(long long amount) {
	this->balance += amount;
	return true;
}
bool Account::operator-(long long amount) {
	if (this->balance < amount) {
		return false;
	}
	else {
		this->balance -= amount;
		return true;
	}
	
}





/*-------------------------------------Main region-------------------------------------------------------------------------------*/

int main() {

	ofstream out("ADMIN.txt");
	string s;
	if (out.is_open()) {
		out << "TRANSACTION_HISTORY" << "\n";
		out << "ID" << "\t" << "CARD_NUM" << "\t" << "TYPES" << "\t" << "AMOUNT" << "\t" << "ETC" << "\n";
	}

	Bank* HN_bank = new Bank("HN은행");
	Bank* JS_bank = new Bank("JS은행");
	Bank* HG_bank = new Bank("HG은행");
	Bank* SH_bank = new Bank("SH은행");

	Account* account_1 = new Account("석하늘", HN_bank, 202321012, 1000000);
	Account* account_2 = new Account("김재석", JS_bank, 202111036, 2000000);
	Account* account_3 = new Account("김재석", JS_bank, 202111036, 0);//Second account at same bank
	Account* account_4 = new Account("성현규", HG_bank, 202111095, 3000000);
	Account* account_5 = new Account("조수호", SH_bank, 202111178, 4000000);


	SingleBank_ATM* atm_SingUnil = new SingleBank_ATM(HN_bank, 0);
	SingleBank_ATM* atm_SingBili = new SingleBank_ATM(JS_bank, 1);
	MultiBank_ATM* atm_MultUnil = new MultiBank_ATM(HG_bank, 0);
	MultiBank_ATM* atm_MultBili = new MultiBank_ATM(SH_bank, 1);

	atm_MultUnil->Add_connected_bank_list(SH_bank);
	atm_MultBili->Add_connected_bank_list(HG_bank);


	//Execute an atm what you want
	atm_MultBili->Start_ATM();


	return 0;
}
