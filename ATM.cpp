#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <iomanip>

using namespace std;


class ATM;
class Bank;
class Account;
class SingleBank_ATM;
class MultiBank_ATM;
vector<SingleBank_ATM*> SingleBank_ATM_list;
vector<MultiBank_ATM*> MultiBank_ATM_list;
vector<Account*> Account_list;


bool isNumber(const string& str, int  only_positive = 0) {
	for (char const& c : str) {
		if (isdigit(c) == 0) {
			if (only_positive) {
				return false;
			}
			else {
				if (c != '-') {
					return false;
				}
				else if (c == '-' && !isdigit(str[str.size() - 1])) {
					return false;
				}
			}
		}
	}
	return true;
}

template< typename T>
T Cin(int _language, int _is_account_num);


/*-------------------------------------Class region-------------------------------------------------------------------------------*/

class ATM {
private:
	static long long atm_count;
	const long long serial_number;
	const long long admin_card_number;
	const long long language_type; // Unilingual = 0, Bilingual = 1
	int language; // English = 1, Korean = 2

	long long number_of_available_cash_list[4]; // {number_of_cash1000, number_of_cash5000, number_of_cash10000, number_of_cash50000}
	long long available_cash = 0; // atm에 있는 현금 총액
	vector<long long> receive_check_list;
	const long long cash_list[4] = { 1000, 5000, 10000, 50000 };
	long long receive_check;

	const string file_name;
	long long Transaction_ID;

	bool Authorization();
	int Menu(int LANG);
	void Admin();
	void Admin_SAVE_TEXT(int TYPE, long long DEST_ACCOUNT, long long AMOUNT, long long FEE, string ETC);
	void Deposit();
	void Withdraw();
	void Transfer();
	bool Allowed_check(long long check);
	bool Account_Exist(long long account_number);
	void Cancel_Button();
	Bank* Call_Bank_of_account(long long account_number);
protected:
	Bank* primary_bank;
	long long _account_number;
	virtual bool Available_account() = 0;
public:
	ATM(Bank* _primary_bank, long long _language_type, long long _number_of_cash1000, long long _number_of_cash5000, long long _number_of_cash10000, long long _number_of_cash50000);
	void Start_ATM();
	int Call_language_type();
	long long Call_serial_number();
	string Call_primary_bank_name();
	long long Call_available_cash();
	vector<long long> Call_number_of_available_cash();
};


class MultiBank_ATM : public ATM {
private:
	bool Available_account();
public:
	MultiBank_ATM(Bank* _primary_bank, long long _language_type, long long _number_of_cash1000, long long _number_of_cash5000, long long _number_of_cash10000, long long _number_of_cash50000)
		: ATM(_primary_bank, _language_type, _number_of_cash1000, _number_of_cash5000, _number_of_cash10000, _number_of_cash50000) {};
};

class SingleBank_ATM : public ATM {
private:
	bool Available_account();
public:
	SingleBank_ATM(Bank* _primary_bank, long long _language_type, long long _number_of_cash1000, long long _number_of_cash5000, long long _number_of_cash10000, long long _number_of_cash50000)
		: ATM(_primary_bank, _language_type, _number_of_cash1000, _number_of_cash5000, _number_of_cash10000, _number_of_cash50000) {};
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
	const string user_name;
	Bank* bank;
	const long long account_number;
	const long long password;
	long long balance; //control only at Constructor, Input_balance and output_balance functions.
	friend bool Bank::Input_balance(long long _account_num, long long _fund); //set freind function. control balance at this function of Bank class
	friend bool Bank::Output_balance(long long _account_num, long long _fund); //set freind function. control balance at this function of Bank class
public:
	Account(string _user_name, Bank* _bank, long long _account_num, long long _password, long long _balance);
	~Account();

	string Call_user_name();
	Bank* Call_bank();
	long long Call_account_number();
	long long Call_password();
	long long Call_balance();
	bool operator+(long long amount);
	bool operator-(long long amount);

	bool new_account_is_valid;
};


long long ATM::atm_count = 0;

vector<Bank*> bank_list; //Global variables



/*-------------------------------------ATM region-------------------------------------------------------------------------------*/
ATM::ATM(Bank* _primary_bank, long long _language_type, long long _number_of_cash1000, long long _number_of_cash5000, long long _number_of_cash10000, long long _number_of_cash50000)
	: serial_number(atm_count + 100000),
	language_type(_language_type),
	primary_bank(_primary_bank),
	admin_card_number(atm_count + 100000),
	file_name(to_string(atm_count + 100000) + ".txt")
{
	_primary_bank->Add_atm_list(this);
	atm_count++;
	Transaction_ID = 0;
	long long _numbers_of_cash[4] = { _number_of_cash1000, _number_of_cash5000, _number_of_cash10000, _number_of_cash50000 };
	for (int i = 0; i < 4; i++) {
		number_of_available_cash_list[i] = _numbers_of_cash[i];
		available_cash += _numbers_of_cash[i] * cash_list[i];
	}


	ofstream out(file_name, ios::trunc | ios::out);
	if (out.is_open()) {
		out << "TRANSACTION_HISTORY" << "\n" << "ID" << "\t" << "WITH_CARD_NUM" << "\t" << "DEPOS_CARD_NUM" << "\t" << "TYPES" << "\t" << "AMOUNT" << "\t" << "FEE" << "\t" << "ETC" << "\n";
	}
	out.close();

}

bool ATM::Authorization() {
	int allowed_num_enter_password = 3;
	long long entered_password;
	long long real_password = Call_Bank_of_account(_account_number)->Find_corresponding_account(_account_number)->Call_password();
	for (int i = 0; i < allowed_num_enter_password; i++) {
		if (language == 1) {
			cout << "Please enter the password" << endl;
		}
		else {
			cout << "비밀번호를 입력해주세요" << endl;
		}
		entered_password = Cin<long long>(language, 0);
		if (entered_password < 0) {//cancel is called
			throw 1;
		}
		else if (entered_password == real_password) {
			if (language == 1) {
				cout << "The password is correct" << endl;
			}
			else {
				cout << "비밀번호가 일치합니다" << endl;
			}
			return true;
		}
		else {
			if (language == 1) {
				cout << "The password is incorrect ( " << i + 1 << "/" << allowed_num_enter_password << " )" << endl;
				cout << "If you get the password wrong " << allowed_num_enter_password << " times, the ATM will be shut down" << endl;
			}
			else {
				cout << "비밀번호가 일치하지 않습니다 ( " << i + 1 << "/" << allowed_num_enter_password << " )" << endl;
				cout << "비밀번호를 " << allowed_num_enter_password << " 번 틀릴 경우 ATM이 종료됩니다" << endl;
			}
		}
	}
	return false;
}

void ATM::Start_ATM() {

	/*=================== Enter the account number ===================*/
	cout << "Please enter your card(account) number" << endl;
	_account_number = Cin<long long>(1, 1);
	/*================================================================*/
	/*==================== Select the Language ====================*/
	// if ATM is bilingual
	if (language_type == 1) {
		cout << "Please select the language" << endl;
		cout << "1. English" << endl;
		cout << "2. 한국어" << endl;
		language = Cin<int>(1, 0);
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

	if (Available_account() == false) {//Check that the account number is valid
		if (language_type == 1) {
			cout << "Please write available account number" << endl;
		}
		else {
			cout << "유효한 계좌번호를 입력해주세요" << endl;
		}
		return;
	}
	/*======================= User Authorization ======================*/
	if (Authorization() == false) {
		if (language == 1) {
			cout << "The ATM is shut down" << endl;
		}
		else {
			cout << "ATM이 종료됩니다" << endl;
		}
		return;
	}
	/*=================================================================*/

	while (1) {
		try { //when cancel is pushed, the throw instance instancely called.
			int menu_type = Menu(language);

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
			/*==Exit==*/
			else if (menu_type == 4) {
				break;
			}

			/*==Cancel==*/
			else if (menu_type < 0) {//Cancel is called, when the input parameter is negative.
				throw 1;
			}
		}
		catch (int _void) {
			Cancel_Button();
			continue;
		}
	}
}

void ATM::Admin_SAVE_TEXT(int _TYPE, long long DEST_ACOUNT, long long _AMOUNT, long long FEE, string ETC) {

	string SAVE_MENU_NAME;
	ofstream out(file_name, ios::app);

	if (_TYPE == 1) {
		SAVE_MENU_NAME = "Depos";
		if (out.is_open())
		{
			string _str = "000000000000";
			out << Transaction_ID << "\t" << "     X    " << "\t" << _str.substr(0, 12 - to_string(DEST_ACOUNT).size()) << DEST_ACOUNT << "\t"
				<< SAVE_MENU_NAME << "\t" << _AMOUNT << "\t" << FEE << "\t" << ETC << "\n";
			Transaction_ID++;
		}
		out.close();
		return;
	}
	else if (_TYPE == 2) {
		SAVE_MENU_NAME = "Withd";
		if (out.is_open())
		{
			out << Transaction_ID << "\t" << _account_number << "\t" << "     X    " << "\t"
				<< SAVE_MENU_NAME << "\t" << _AMOUNT << "\t" << FEE << "\t" << ETC << "\n";
			Transaction_ID++;
		}
		out.close();
		return;
	}
	else if (_TYPE == 3) {
		SAVE_MENU_NAME = "Trans";
		if (out.is_open())
		{
			string _str = "000000000000";
			out << Transaction_ID << "\t" << _account_number << "\t" << _str.substr(0, 12 - to_string(DEST_ACOUNT).size()) << DEST_ACOUNT << "\t"
				<< SAVE_MENU_NAME << "\t" << _AMOUNT << "\t" << FEE << "\t" << ETC << "\n";
			Transaction_ID++;
		}
		out.close();
		return;
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
	for (int i = 0; i < (int)bank_list.size(); i++) {
		bank = bank_list[i];
		if (bank->Exist_account(account_number)) {
			return true;
		}
	}
	return false;
}

Bank* ATM::Call_Bank_of_account(long long account_number) {
	Bank* bank;
	for (int i = 0; i < (int)bank_list.size(); i++) {
		bank = bank_list[i];
		if (bank->Exist_account(account_number)) {
			return bank;
		}
	}
	return nullptr;
}


int ATM::Menu(int LANG) {
	int MENU_SELECT_NUMBER;
	int var;

	if (LANG == 1) {
		cout << "Enter the Number to Select the menu" << endl;
		cout << "1. Deposit" << endl;
		cout << "2. Withdraw" << endl;
		cout << "3. Transfer" << endl;
		cout << "4. Exit" << endl;
		cout << "If you want to cancel this session, write -1 " << endl;
		MENU_SELECT_NUMBER = Cin<int>(language, 0);
		var = MENU_SELECT_NUMBER;
	}
	else
	{
		cout << "이용하실 메뉴의 번호를 눌러주세요" << endl;
		cout << "1. 입금" << endl;
		cout << "2. 출금" << endl;
		cout << "3. 송금" << endl;
		cout << "4. 종료" << endl;
		cout << "취소를 원하시면, -1 을 적어주세요" << endl;
		MENU_SELECT_NUMBER = Cin<int>(language, 0);
		var = MENU_SELECT_NUMBER;
	}

	return var;
}

void ATM::Admin() {
	int ADMIN_MENU_SELECT_NUMBER;
	int ADMIN_CANCEL_SWITCH;
	int ADMIN_WHILE_SWITCH;


	while (1) {
		if (language == 1) {
			cout << "Entering Administrator menu" << endl;
			cout << "5. Transaction History" << endl;

			ADMIN_MENU_SELECT_NUMBER = Cin<int>(language, 0);
			if (ADMIN_MENU_SELECT_NUMBER < 0) {//cancel is called
				throw 1;
			}
			else if (ADMIN_MENU_SELECT_NUMBER == 5) {
				ifstream file(file_name);
				if (file) {
					char c;
					while (file.get(c)) {
						cout.put(c);
					}
				}
				else {
					cout << "Error while openning the File " << file_name << endl;
					cout << "Press any keys to restart the session" << endl;
					ADMIN_WHILE_SWITCH = Cin<int>(language, 0);
				}
				cout << "Press any keys to continue" << endl;
				ADMIN_CANCEL_SWITCH = Cin<int>(language, 0);
				return;
			}
			else {
				cout << "Wrong Number" << endl;
			}
		}

		if (language == 2) {
			cout << "관리자 메뉴로 진입합니다" << endl;
			cout << " 5. 거래내역" << endl;

			ADMIN_MENU_SELECT_NUMBER = Cin<int>(language, 0);

			if (ADMIN_MENU_SELECT_NUMBER == 5) {
				ifstream file(file_name);
				if (file) {
					char c;
					while (file.get(c)) {
						cout.put(c);
					}
				}
				else {
					cout << "Error while openning the File " << file_name << endl;
					cout << "아무 키나 눌러 세션을 재 시작해 주세요" << endl;
					ADMIN_WHILE_SWITCH = Cin<int>(language, 0);
				}
				cout << "계속하시려면 아무 키나 눌러주세요" << endl;
				ADMIN_CANCEL_SWITCH = Cin<int>(language, 0);
				return;
			}
			else {
				cout << "틀린 번호입니다" << endl;
			}
		}

	}
	ADMIN_CANCEL_SWITCH = 0;

	return;
}

void ATM::Deposit() {
	/*Set the Source Bank*/
	Bank* source_bank = Call_Bank_of_account(_account_number);

	int deposit_type;// Cash or Check?
	while (1) {
		if (language == 1) {
			cout << "Please select the type of deposit" << endl;
			cout << "1. Cash deposit" << endl;
			cout << "2. Check deposit" << endl;
		}
		else {
			cout << "입금하는 화폐의 종류를 택하여 주세요" << endl;
			cout << "1. 지폐 예금" << endl;
			cout << "2. 수표 예금" << endl;
		}
		deposit_type = Cin<int>(language, 0);
		if (deposit_type == 1) {
			if (language == 1) {
				cout << "Cash deposit is selected" << endl;
			}
			else {
				cout << "지폐 입금이 선택되었습니다" << endl;
			}
			break;
		}
		else if (deposit_type == 2) {
			if (language == 1) {
				cout << "Check deposit is selected" << endl;
			}
			else {
				cout << "수표 입금이 선택되었습니다" << endl;
			}
			break;
		}
		else if (deposit_type < 0) {//cancel is called
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


	Bank* destination_bank = source_bank;
	long long destination_account_number = _account_number;

	// Regradless of monry type, fee is same. For non-primary, fee is paid additonally.
	long long account_deposit_fee;
	if (source_bank == primary_bank) {
		account_deposit_fee = 0;
	}
	else {
		account_deposit_fee = 1000;
	}

	/*======================= User Authorization ======================*/
	if (Authorization() == false) {
		if (language == 1) {
			cout << "The ATM is shut down" << endl;
		}
		else {
			cout << "ATM이 종료됩니다" << endl;
		}
		return;
	}
	/*=================================================================*/

	if (deposit_type == 1) {

		/*=================== Recieve Cash Deposit Payment ===================*/
		long long pay = 0;
		long long number_of_cash_list[4];
		while (1) {
			if (language == 1) {
				cout << "Please pay the amount that you want to cash deposit including deposit fee KRW " << account_deposit_fee << endl;
			}
			else {
				cout << "현금 입금하실 금액에 입금 수수료 " << account_deposit_fee << " 원을 더하여 지불해주세요" << endl;
			}

			for (int i = 0; i < 4; i++) {
				if (language == 1) {
					cout << "Please enter the number of KRW " << cash_list[i] << " to be paid" << endl;
				}
				else {
					cout << "지불하실" << cash_list[i] << "원의 개수를 입력해주세요" << endl;
				}
				number_of_cash_list[i] = Cin<long long>(language, 0);
				if (number_of_cash_list[i] < 0) {//Cancle is called
					throw 1;
				}
			}
			long long sum = 0;
			for (int i = 0; i < 4; i++) {
				sum += number_of_cash_list[i];
			}
			if (sum > 50) {
				if (language == 1) {
					cout << "The total number of paper cash exceeded 50" << endl;
				}
				else {
					cout << "총 지폐의 개수가 50장을 초과하였습니다" << endl;
				}
				continue;
			}

			for (int i = 0; i < 4; i++) {
				pay += cash_list[i] * number_of_cash_list[i];
			}



			if (pay <= account_deposit_fee) {
				if (language == 1) {
					cout << "Your payment is less than or equal to the deposit fee" << endl;
				}
				else {
					cout << "지불하신 금액이 입금 수수료보다 작거나 같습니다" << endl;
				}
				continue;
			}
			else {
				if (language == 1) {
					cout << "The total amount that you paid is KRW " << pay << endl;
					cout << "The amount of deposit: KRW " << pay - account_deposit_fee << endl;
					cout << "Deposit fee: KRW " << account_deposit_fee << endl;
				}
				else {
					cout << "지불하신 총 금액은 " << pay << " 원입니다" << endl;
					cout << "입금 금액: " << pay - account_deposit_fee << " 원" << endl;
					cout << "입금 수수료: " << account_deposit_fee << " 원" << endl;
				}
				available_cash += pay;
				for (int i = 0; i < 4; i++) {
					number_of_available_cash_list[i] += number_of_cash_list[i]; // update the number of each amount of cash, 1000, 5000 etc.
				}
				break;
			}
		}
		destination_bank->Input_balance(destination_account_number, pay - account_deposit_fee);


		/*============================ End Session ============================*/
		if (language == 1) {
			cout << "The cash of KRW " << pay - account_deposit_fee << " to " << destination_bank->Call_bank_name() << " " << destination_account_number << " is deposited successfully" << endl;
		}
		else {
			cout << pay - account_deposit_fee << " 원이 " << destination_bank->Call_bank_name() << " " << destination_account_number << " 으로 입금 되었습니다" << endl;
		}
		Admin_SAVE_TEXT(1, destination_account_number, pay - account_deposit_fee, account_deposit_fee, "bill");
		/*=====================================================================*/
	}

	if (deposit_type == 2) {

		long long pay;
		long long sum_amount_of_check = 0;
		int num_check;
		while (1) {
			if (language == 1) {
				cout << "Enter the number of checks that you want to depsoit" << endl;
			}
			else {
				cout << "입금할 수표의 개수를 입력해주세요" << endl;
			}
			num_check = Cin<int>(language, 0);
			if (num_check > 30) {
				if (language == 1) {
					cout << "The number of checks exceeded 30" << endl;
				}
				else {
					cout << "총 수표의 개수가 30장을 초과하였습니다" << endl;
				}
				continue;
			}
			else if (num_check < 0) {//Cancel is called
				throw 1;
			}
			else {
				break;
			}
		}


		for (int i = 0; i < num_check; i++) {
			if (language == 1) {
				cout << "Please enter the (" << i + 1 << "/" << num_check << ") th amount of check" << endl;
			}
			else {
				cout << "(" << i + 1 << "/" << num_check << ") 번째 수표의 금액을 입력해주세요" << endl;
			}

			long long check_amount;
			check_amount = Cin<long long>(language, 0);
			if (Allowed_check(check_amount) == false) {
				i--;
			}
			else if (check_amount < 0) {//cancel is called
				throw 1;
			}
			else {
				receive_check_list.push_back(check_amount);
				sum_amount_of_check += check_amount;
			}
		}


		int fee;
		while (1) {
			if (account_deposit_fee == 1000) {
				if (language == 1) {
					cout << "Please pay deposit fee with cash KRW 1000" << endl;
					cout << "(Enter the number, 1000)" << endl;
				}
				else {
					cout << "입금 수수료 현금 1000 원을 지불해주세요" << endl;
					cout << "(숫자 1000을 입력)" << endl;
				}
				fee = Cin<int>(language, 0);
				if (fee != 1000) {
					if (language == 1) {
						cout << "The amount of deposit fee is incorrect" << endl;
					}
					else {
						cout << "입금 수수료의 금액이 올바르지 않습니다" << endl;
					}
					continue;
				}
				else if (fee < 0) {//cancel is called
					throw 1;
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}

		pay = sum_amount_of_check + account_deposit_fee;


		if (language == 1) {
			cout << "The total amount that you paid is KRW " << pay << endl;
			cout << "The amount of deposit: KRW " << pay - account_deposit_fee << endl;
			cout << "Deposit fee: KRW " << account_deposit_fee << endl;
		}
		else {
			cout << "지불하신 총 금액은 " << pay << " 원입니다" << endl;
			cout << "입금 금액: " << pay - account_deposit_fee << " 원" << endl;
			cout << "입금 수수료: " << account_deposit_fee << " 원" << endl;
		}


		number_of_available_cash_list[0] += 1;
		available_cash += account_deposit_fee;


		destination_bank->Input_balance(destination_account_number, pay - account_deposit_fee);

		/*============================ End Session ============================*/
		if (language == 1) {
			cout << "The check of KRW " << pay - account_deposit_fee << " to " << destination_bank->Call_bank_name() << " " << destination_account_number << " is deposited successfully" << endl;
		}
		else {
			cout << pay - account_deposit_fee << " 원이 " << destination_bank->Call_bank_name() << " " << destination_account_number << "에 입금 되었습니다" << endl;
		}
		Admin_SAVE_TEXT(1, destination_account_number, pay, account_deposit_fee, "check");
		/*=====================================================================*/
	}
}

void ATM::Withdraw() {
	// Source bank <-- card bank
	Bank* source_bank = Call_Bank_of_account(_account_number);

	long long account_withdraw_fee;
	if (source_bank == primary_bank) {
		account_withdraw_fee = 1000;
	}
	else {
		account_withdraw_fee = 2000;
	}

	/*======================= User Authorization ======================*/
	if (Authorization() == false) {
		if (language == 1) {
			cout << "The ATM is shut down" << endl;
		}
		else {
			cout << "ATM이 종료됩니다" << endl;
		}
		return;
	}
	/*=================================================================*/

	int num;
	long long take;


	for (int i = 0; i < 3; i++) {
		long long withdraw_cash_list[4];
		while (1) {
			if (language == 1) {
				cout << "The balance of source account: KRW " << source_bank->Call_balance(_account_number) << endl;
				cout << "In the case of withdrawal, the withdraw fee KRW " << account_withdraw_fee << " will be paid from the source account" << endl;
				cout << "Please enter the amount of withdrawal" << endl;
			}
			else {
				cout << "출금 계좌의 잔액: " << source_bank->Call_balance(_account_number) << " 원" << endl;
				cout << "출금 수수료 " << account_withdraw_fee << " 원이 출금 계좌에서 추가로 지불됩니다" << endl;
				cout << "출금하려는 금액을 입력하여주세요" << endl;
			}
			take = Cin<long long>(language, 0);
			if (take < 0) {//Cancel is called
				throw 1;
			}
			else if (take > 500000) {
				if (language == 1) {
					cout << "The amount of withdrawal is over KRW 500000" << endl;
					cout << "Please make sure that the amount of withdrawal does not exceed KRW 500000" << endl;
				}
				else {
					cout << "출금 금액이 500000 원을 초과했습니다" << endl;
					cout << "출금 금액이 500000 원을 넘지 않도록 해주세요" << endl;
				}
			}
			else if (take + account_withdraw_fee > source_bank->Call_balance(_account_number)) {
				if (language == 1) {
					cout << "The balance of source account is not sufficient" << endl;
				}
				else {
					cout << "출금 계좌의 잔액이 부족합니다" << endl;
				}
				continue;
			}
			else if (available_cash < take) {
				if (language == 1) {
					cout << "The ATM dose not have enough cash to withdraw" << endl;
				}
				else {
					cout << "ATM 내부의 현금 부족으로 출금을 지원할 수 없습니다" << endl;
				}
				continue;
			}
			else {
				long long took = take;
				for (int i = 0; i < 4; i++) {
					int j = 3 - i;
					withdraw_cash_list[j] = took / cash_list[j];
					if (withdraw_cash_list[j] >= number_of_available_cash_list[j]) {
						withdraw_cash_list[j] = number_of_available_cash_list[j];
					}
					took -= withdraw_cash_list[j] * cash_list[j];
				}

				if (took != 0) {
					if (language == 1) {
						cout << "The ATM dose not have enough amount of paper cash to withdraw" << endl;
					}
					else {
						cout << "ATM 내부의 출금 가능한 지폐가 부족합니다" << endl;
					}
					continue;
				}


				if (language == 1) {
					cout << "The total amount of withdrawal from the source bank: KRW " << take + account_withdraw_fee << endl;
					cout << "The amount of withdrawal: KRW " << take << endl;
					cout << "Withdrawal fee: KRW " << account_withdraw_fee << endl;
				}
				else {
					cout << "계좌에서 출금된 총 금액: " << take + account_withdraw_fee << " 원" << endl;
					cout << "출금 금액: " << take << " 원" << endl;
					cout << "출금 수수료: " << account_withdraw_fee << " 원" << endl;
				}
				break;
			}
		}

		for (int i = 0; i < 4; i++) {
			number_of_available_cash_list[i] -= withdraw_cash_list[i];
			if (language == 1) {
				cout << "The " << withdraw_cash_list[i] << " paper cash with amount of KRW " << cash_list[i] << " was withdrawn" << endl;
			}
			else {
				cout << "현금 " << cash_list[i] << " 원 " << withdraw_cash_list[i] << " 개가 인출되었습니다" << endl;
			}
		}
		available_cash -= take;
		source_bank->Output_balance(_account_number, take + account_withdraw_fee);

		/*============================ End Session ============================*/
		if (language == 1) {
			cout << "The withdraw of KRW " << take + account_withdraw_fee << " from " << source_bank->Call_bank_name() << " " << _account_number << " was performed successfully" << endl;
		}
		else {
			cout << take + account_withdraw_fee << " 원이 " << source_bank->Call_bank_name() << " " << _account_number << " 에서 출금 되었습니다" << endl;
		}
		Admin_SAVE_TEXT(2, 0, take + account_withdraw_fee, account_withdraw_fee, "bill");
		/*=====================================================================*/


		if (i == 2) {
			return;
		}
		else {
			while (1) {

				if (language == 1) {
					cout << "(1) Withdraw from the account once again or (2) End the session" << endl;
				}
				else {
					cout << "(1) 한 번 더 출금 하시겠습니까 아니면 (2) 세션을 종료하시겠습니까" << endl;
				}
				num = Cin<int>(language, 0);

				if (num == 1) {
					if (language == 1) {
						cout << "The withdrawal session will start again (" << i << "/" << 3 << ")" << endl;
					}
					else {
						cout << "한 번 더 출금을 진행하겠습니다" << endl;
					}
					break;
				}
				else if (num == 2) {
					if (language == 1) {
						cout << "The withdrawal session is ended" << endl;
					}
					else {
						cout << "출금 세션이 종료되었습니다" << endl;
					}
					return;
				}
				else if (num < 0) {//Cancel is called
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
		}
	}



	return;//when successfully ended
}

void ATM::Transfer() {
	long long copy_account_number= _account_number;
	long long source_account_number;
	Bank* source_bank = nullptr;
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
		transfer_type = Cin<int>(language, 0);
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
			}
			else {
				cout << "계좌 이체가 선택되었습니다" << endl;
			}
			while (1) {
				if (language == 1) {
					cout << "Please enter the source account number" << endl;
				}
				else {
					cout << "출금 계좌 번호를 입력해주세요" << endl;
				}
				copy_account_number = _account_number;
				source_account_number = Cin<long long>(language, 1);
				_account_number = source_account_number;
				if (source_account_number < 0) {
					_account_number = copy_account_number;
					throw 1;
				}
				else if (Available_account() == false) {
					if (language == 1) {
						cout << "The account does not exist" << endl;
					}
					else {
						cout << "존재하지 않는 계좌입니다" << endl;
						_account_number = copy_account_number;
						continue;
					}
				}
				else {
					source_bank = Call_Bank_of_account(source_account_number);
					break;
				}
			}
			if (language == 1) {
				cout << "The withdrawal account is " << source_bank->Call_bank_name() << " " << _account_number << endl;
			}
			else {
				cout << "출금 계좌는 " << source_bank->Call_bank_name() << " " << _account_number << " 입니다" << endl;
			}
			break;
		}
		else if (transfer_type < 0) {//Cancel is called
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
		destination_account_number = Cin<long long>(language, 1);
		if (destination_account_number < 0) {//Cancel is called
			_account_number = copy_account_number;
			throw 1;
		}
		else if (_account_number == destination_account_number) {
			if (language == 1) {
				cout << "The source account and the destination account is same" << endl;
			}
			else {
				cout << "출금 계좌와 입금 계좌가 같습니다" << endl;
			}
			continue;
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
				cout << "The destination account number does not exist" << endl;
			}
			else {
				cout << "입금 계좌 번호가 존재하지 않습니다" << endl;
			}
			continue;
		}
	}
	Bank* destination_bank = Call_Bank_of_account(destination_account_number);
	/*=================================================================*/

	/*======================= User Authorization ======================*/
	if (transfer_type == 2) {
		if (language == 1) {
			cout << "Please enter the password of source account" << endl;
		}
		else {
			cout << "출금 계좌의 비밀번호를 입력해주세요" << endl;
		}
	}
	if (Authorization() == false) {
		if (language == 1) {
			cout << "The ATM is shut down" << endl;
		}
		else {
			cout << "ATM이 종료됩니다" << endl;
		}
		_account_number = copy_account_number;
		return;
	}
	/*=================================================================*/

	/*==================================== Transfer ====================================*/
	// if Cash transfer
	if (transfer_type == 1) {

		/*=================== Recieve Cash Transfer Payment ===================*/
		long long number_of_cash_list[4]; // {number_of_cash_1000, number_of_cash_5000, number_of_cash_10000, number_of_cash_50000}
		long long pay = 0;
		long long cash_transfer_fee = 5000;
		while (1) {
			if (language == 1) {
				cout << "Please pay the amount that you want to cash transfer plus cash transfer fee KRW " << cash_transfer_fee << endl;
			}
			else {
				cout << "현금 이체하실 금액에 현금 이체 수수료 " << cash_transfer_fee << " 원을 더하여 지불해주세요" << endl;
			}
			for (int i = 0; i < 4; i++) {
				if (language == 1) {
					cout << "Please enter the number of KRW " << cash_list[i] << " to be paid" << endl;
				}
				else {
					cout << "지불하실" << cash_list[i] << "원의 개수를 입력해주세요" << endl;
				}
				number_of_cash_list[i] = Cin<long long>(language, 0);
				if (number_of_cash_list[i] < 0) {//Cancle is called
					throw 1;
				}
			}
			for (int i = 0; i < 4; i++) {
				pay += cash_list[i] * number_of_cash_list[i];
			}

			if (pay <= cash_transfer_fee) {
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
					cout << "현금 이체 금액: " << pay - cash_transfer_fee << " 원" << endl;
					cout << "현금 이체 수수료: " << cash_transfer_fee << " 원" << endl;
				}
				for (int i = 0; i < 4; i++) {
					number_of_available_cash_list[i] += number_of_cash_list[i];
				}
				available_cash += pay;
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
		Admin_SAVE_TEXT(3, destination_account_number, pay - cash_transfer_fee, cash_transfer_fee, "bill");
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
			amount_transfer = Cin<long long>(language, 0);
			if (amount_transfer < 0) {//Cancel is called
				_account_number = copy_account_number;
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
		Admin_SAVE_TEXT(3, destination_account_number, amount_transfer, account_transfer_fee, "account");
		_account_number = copy_account_number;
		return;
		/*=====================================================================*/

	}
	/*==================================================================================*/
	_account_number = copy_account_number;
	return;
}

void ATM::Cancel_Button() {
	if (language == 1) {
		cout << "The cancel button is pressed. Exit the current session." << endl;
	}
	else {
		cout << "취소 버튼을 눌렀습니다. 진행중인 세션을 종료합니다." << endl;
	}
}

int ATM::Call_language_type() {
	return (int)language_type;
}
long long ATM::Call_serial_number() {
	return serial_number;
}
string ATM::Call_primary_bank_name() {
	return primary_bank->Call_bank_name();
}
long long ATM::Call_available_cash() {
	return available_cash;
}
vector<long long> ATM::Call_number_of_available_cash() {
	vector<long long> result;
	for (int i = 0; i < 4; ++i) {
		result.push_back(number_of_available_cash_list[i]);
	}
	return result;
}
/*------------------------------MultiBank_ATM region-----------------------------------------------------------------------------*/
bool MultiBank_ATM::Available_account() {
	for (int i = 0; i < (int)bank_list.size(); i++) {
		if (bank_list[i]->Exist_account(_account_number)) {
			return true;
		}
	}
	return false;
}


/*------------------------------SingleBank_ATM region-----------------------------------------------------------------------------*/
bool SingleBank_ATM::Available_account() {
	for (int i = 0; i < (int)bank_list.size(); i++) {
		if (bank_list[i]->Exist_account(_account_number)) {
			if (bank_list[i] == primary_bank) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
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
	for (int i = 0; i < (int)account_list.size(); i++) {
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
	return *Find_corresponding_account(_account_num) + _fund;
}
bool Bank::Output_balance(long long _account_num, long long _fund) {
	return *Find_corresponding_account(_account_num) - _fund;
}

Account* Bank::Find_corresponding_account(long long _account_num) {
	for (int i = 0; i < (int)account_list.size(); i++) {
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





Account::Account(string _user_name, Bank* _bank, long long _account_number, long long _password, long long _balance)
	: user_name(_user_name),
	bank(_bank),
	password(_password),
	balance(_balance),
	account_number(_account_number)
{
	_bank->Add_account_list(this);
	new_account_is_valid = true;
	for (auto& e : Account_list) {
		if (e->Call_account_number() == _account_number) {
			new_account_is_valid = false;
			cout << "ERROR::The new account number is overlaping with old one" << endl;
		}
	}

}

Account::~Account() {}

string Account::Call_user_name() {
	return user_name;
}
Bank* Account::Call_bank() {
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
template <typename T>
T Cin(int _language, int _is_account_num ) {
	T result;
	while (1) {
		string input_var;
		cin >> input_var;
		if (input_var == "atm" || input_var == "Atm" || input_var == "ATM") {
			for (int i = 0; i < SingleBank_ATM_list.size(); ++i) {
				cout << "ATM [SN: " << SingleBank_ATM_list[i]->Call_serial_number() << "] remaining cash : " << SingleBank_ATM_list[i]->Call_available_cash()
					<< ", Number of cash : ( " << SingleBank_ATM_list[i]->Call_number_of_available_cash()[0] << ", " << SingleBank_ATM_list[i]->Call_number_of_available_cash()[1]
					<< ", " << SingleBank_ATM_list[i]->Call_number_of_available_cash()[2] << ", " << SingleBank_ATM_list[i]->Call_number_of_available_cash()[3] << " )" << endl;
			}
			for (int i = 0; i < MultiBank_ATM_list.size(); ++i) {
				cout << "ATM [SN: " << MultiBank_ATM_list[i]->Call_serial_number() << "] remaining cash : " << MultiBank_ATM_list[i]->Call_available_cash()
					<< ", Number of cash : ( " << MultiBank_ATM_list[i]->Call_number_of_available_cash()[0] << ", " << MultiBank_ATM_list[i]->Call_number_of_available_cash()[1]
					<< ", " << MultiBank_ATM_list[i]->Call_number_of_available_cash()[2] << ", " << MultiBank_ATM_list[i]->Call_number_of_available_cash()[3] << " )" << endl;
			}
			cout << endl;
		}
		else if (input_var == "account" || input_var == "Account" || input_var == "ACCOUNT") {
			for (int i = 0; i < Account_list.size(); ++i) {
				string _str = "000000000000";
				cout << "Account [Bank: " << Account_list[i]->Call_bank()->Call_bank_name() << ", No: "
					<< _str.substr(0, 12 - to_string(Account_list[i]->Call_account_number()).size()) << Account_list[i]->Call_account_number()//12 digit num
					<< ", Owner: " << Account_list[i]->Call_user_name() << ", password: " << Account_list[i]->Call_password() << "] balance: " << Account_list[i]->Call_balance() << endl;
			}
			cout << endl;
		}
		else if (isNumber(input_var, _is_account_num)) {
			if (_is_account_num) {
				if (input_var.size() != 12 && input_var.size() != 6) {
					if (_language == 1) {
						cout << "The number is not 12 digits.\nPlease write again." << endl;
						continue;
					}
					else if (_language == 2) {
						cout << "숫자가 12자리가 아닙니다.\n다시 입력해주세요." << endl;
						continue;
					}
					else {
						cout << "Warning::Code has serious error. You should fix the codes. Errorcode:27dds3bs638s" << endl;
						continue;
					}
				}
			}

			if (is_same<T, int>::value) {
				result = stoi(input_var);
			}
			else if (is_same<T, long long>::value) {
				result = stoll(input_var);
			}
			else {
				cout << "ERROR:ohffq29d8h2uhd" << endl;
			}
			break;
		}
		else if (isNumber(input_var)) {
			if (is_same<T, int>::value) {
				result = stoi(input_var);
			}
			else if (is_same<T, long long>::value) {
				result = stoll(input_var);
			}
			else {
				cout << "ERROR:ohffq29d8h2uhd" << endl;
			}
			break;
		}
		else {
			if (_language == 2) {
				cout << "에러::올바르지 않은 입력이 들어왔습니다.\n다시 입력해주세요." << endl;
			}
			else {
				cout << "ERROR::Invalid text.\nPlease write again." << endl;
			}
		}
	}
	return result;
}

void Construct_part() {
	cout << "=====Start Construct_part=====\n==============================" << endl;


	/*======Bank construct======*/
	while (1) {
		cout << "===Bank Construct===" << endl;
		cout << "   Continue this part : 1 \n   End this part : 2" << endl;
		int var;
		cout << "answer : ";

		try {
			cin >> var;
			if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
			if (var == 1) {
				cout << "\nPlease write new Bank name." << endl;
				string new_bank_name = "";
				cout << "answer : ";
				cin >> new_bank_name;
				cout << endl;
				if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
				new Bank(new_bank_name);
			}
			else if (var == 2) {
				cout << "End this part.\n" << endl;
				break;
			}
			else {//error
				throw 1;
			}
		}
		catch (bad_alloc& e) {
			cout << e.what() << endl;
			cout << "PLease write again\n" << endl;
			continue;
		}
		catch (int _void) {
			cout << "Invalid text. Please write again\n" << endl;
			continue;
		}

	}

	/*======Account construct======*/
	while (1) {
		cout << "===Account Construct===" << endl;
		cout << "   Continue this part : 1 \n   End this part : 2" << endl;

		try {
			int var;
			cout << "answer : ";
			cin >> var;
			if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
			if (var == 1) {
				/*===input user name===*/
				cout << "\nPlease write the user name" << endl;
				string username;
				cout << "answer : ";
				cin >> username;
				if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error

				/*===input bank name===*/
				Bank* corr_bank = nullptr;
				string bankname;
				while (1) {
					cout << "\nPlease write the bank name" << endl;
					cout << "answer : ";
					cin >> bankname;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					int i;
					for (i = 0; i < (int)bank_list.size(); i++) {
						if (bank_list[i]->Call_bank_name() == bankname) {
							corr_bank = bank_list[i];
							break;
						}
					}
					if (i == (int)bank_list.size()) {//error
						cout << "Invalid bank name. \rPlease write again.\n" << endl;
						continue;
					}
					else {
						break;
					}
				}


				/*===input account name===*/
				string input_account_num;
				while (1) {
					cout << "\nPlease write the 12 digit account number." << endl;
					cout << "answer : ";
					cin >> input_account_num;
					if (input_account_num.size() != 12) {
						cout << "Invalid format. The number of digits is not correct. \nPlease write again." << endl;
						continue;
					}
					else if (isNumber(input_account_num, 1) == false) {
						cout << "Invalid format. The entered character is not a number. \nPlease write again." << endl;
						continue;
					}
					else {
						break;
					}
				}
				long long account_num_12digit = stoll(input_account_num);

				/*===input password===*/
				cout << "\nPlease write the password which is construced by digit number" << endl;
				long long password;
				cout << "answer : ";
				cin >> password;
				if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error

				/*===input initial balance===*/
				cout << "\nPlease write the initial balance" << endl;
				long long balance;
				cout << "answer : ";
				cin >> balance;
				if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error

				/*===Check information again===*/
				cout << "\nWil you construct an account by upper informations?" << endl;
				cout << "Yes : 1     No : (any other number)" << endl;
				int var_;
				cin >> var_;
				cout << endl;
				if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
				if (var_ != 1) {
					continue;
				}

				/*===new account construct===*/
				Account* new_Account = new Account(username, corr_bank, account_num_12digit, password, balance);
				if (new_Account->new_account_is_valid) {
					Account_list.push_back(new_Account);
					cout << "Account information || user name : " << new_Account->Call_user_name() << " , account number : " << input_account_num
						<< " ,\n correlate bank : " << bankname << " , password : " << new_Account->Call_password()
						<< " ,\n initial balance : " << new_Account->Call_balance() << "\n" << endl;
				}
				else {
					cout << "Delete the new account information. Please write again.\n" << endl;
					delete new_Account;
				}
			}
			else if (var == 2) {
				cout << "End this part\n" << endl;
				break;
			}
			else {
				throw 1;
			}

		}
		catch (bad_alloc& e) {
			cout << e.what() << endl;
			cout << "Please write again" << endl;
		}
		catch (int _void) {
			cout << "Invalid text. Please write again\n" << endl;
			continue;
		}

	}

	/*======ATM construct======*/
	while (1) {
		cout << "===ATM Construct===" << endl;
		cout << "   Continue this part : 1 \n   End this part : 2" << endl;

		try {
			int var;
			cout << "answer : ";
			cin >> var;
			if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
			if (var == 1) {
				/*===input primary bank name===*/
				string primary_bankname;
				Bank* primary_bank = nullptr;
				while (1) {
					cout << "\nPlease write the primary bank name" << endl;
					cout << "answer : ";
					cin >> primary_bankname;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					int i;
					for (i = 0; i < (int)bank_list.size(); i++) {
						if (bank_list[i]->Call_bank_name() == primary_bankname) {
							primary_bank = bank_list[i];
							break;
						}
					}
					if (i == (int)bank_list.size()) {//error
						cout << "Invalid bank name. \rPlease write again.\n" << endl;
						continue;
					}
					else {
						break;
					}
				}

				/*===input cash type===*/
				long long input_cash_list[4] = { 0, 0, 0, 0 };
				long long _var;
				cout << "\nPlease enter the number of cash per cash type." << endl;
				while (1) {
					cout << " KRW 1000  : ";
					cin >> _var;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					if (_var < 0) {
						cout << "Invalid number. Inputed number is negative.\nPlease write again." << endl;
					}
					else {
						input_cash_list[0] = _var;
						break;
					}
				}
				while (1) {
					cout << " KRW 5000  : ";
					cin >> _var;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					if (_var < 0) {
						cout << "Invalid number. Inputed number is negative.\nPlease write again." << endl;
					}
					else {
						input_cash_list[1] = _var;
						break;
					}
				}
				while (1) {
					cout << " KRW 10000 : ";
					cin >> _var;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					if (_var < 0) {
						cout << "Invalid number. Inputed number is negative.\nPlease write again." << endl;
					}
					else {
						input_cash_list[2] = _var;
						break;
					}
				}
				while (1) {
					cout << " KRW 50000 : ";
					cin >> _var;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					if (_var < 0) {
						cout << "Invalid number. Inputed number is negative.\nPlease write again." << endl;
					}
					else {
						input_cash_list[3] = _var;
						break;
					}
				}


				/*===input language type===*/
				int lingual_type;
				while (1) {
					cout << "\nPlease select the unilingual ATM or Bilingual ATM\n   Unilingual ATM : 0\n   Bilingual ATM : 1" << endl;
					cout << "answer : ";
					cin >> lingual_type;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					if (lingual_type == 0 || lingual_type == 1) {
						break;
					}
					else {
						cout << "Invalid number!\nPlease write again.\n" << endl;
						continue;
					}
				}


				/*===input bank type===*/
				int bank_type;
				while (1) {
					cout << "\nPlease select the SingleBank_ATM or MultiBank_ATM\n   SingleBank_ATM : 0\n   MultiBank_ATM : 1" << endl;
					cout << "answer : ";
					cin >> bank_type;
					if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
					if (bank_type == 0 || bank_type == 1) {
						break;
					}
					else {
						cout << "Invalid number!\nPlease write again.\n" << endl;
						continue;
					}
				}

				/*===Check information again===*/
				cout << "\nWil you construct an account by upper informations?" << endl;
				cout << "Yes : 1     No : (any other number)" << endl;
				int var_;
				cin >> var_;
				cout << endl;
				if (cin.fail()) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); throw 1; }//cin error
				if (var_ != 1) {
					continue;
				}

				/*===new ATM Construct===*/
				if (bank_type == 0) {
					SingleBank_ATM_list.push_back(new SingleBank_ATM(primary_bank, lingual_type, input_cash_list[0], input_cash_list[1], input_cash_list[2], input_cash_list[3]));
				}
				else {
					MultiBank_ATM_list.push_back(new MultiBank_ATM(primary_bank, lingual_type, input_cash_list[0], input_cash_list[1], input_cash_list[2], input_cash_list[3]));
				}
			}
			else if (var == 2) {
				cout << "End this session\n" << endl;
				break;
			}
			else {
				throw 1;
			}

		}
		catch (bad_alloc& e) {
			cout << e.what() << endl;
			cout << "Please write agaion" << endl;
		}
		catch (int _void) {
			cout << "Invalid text. Please write again\n" << endl;
			continue;
		}
	}
	cout << "=====End Construct_part=====\n============================\n\n" << endl;
}

void Execution_part() {
	cout << "=====Start Execution_part=====\n==============================" << endl;

	while (1) {
		cout << "\nSelect the ATM what you want to use." << endl;
		cout << "   SingleBank_ATM : 1\n   MultiBank_ATM : 2" << endl;
		int var;

		try {
			var = Cin<int>(1, 0);


			if (var == 1) {
				cout << "SingleBank_ATM list. Please select the number" << endl;
				for (int i = 0; i < (int)SingleBank_ATM_list.size(); i++) {
					cout << "[" << i + 1 << "] SingleBank_ATM : " << ((SingleBank_ATM_list[i]->Call_language_type() == 0) ? ("Unilingual type , ") : ("Bilingual type , "))
						<< "Primary bank name : " << SingleBank_ATM_list[i]->Call_primary_bank_name() << " , Serial number " << SingleBank_ATM_list[i]->Call_serial_number() << endl;
				}
				int foo;
				foo = Cin<int>(1, 0);

				if (foo > (int)SingleBank_ATM_list.size() || foo <= 0) {
					cout << "ERROR:Please write valid number" << endl;
					continue;
				}
				SingleBank_ATM_list[foo - 1]->Start_ATM();
			}
			else if (var == 2) {
				cout << "MultiBank_ATM list. Please select the number" << endl;
				for (int i = 0; i < (int)MultiBank_ATM_list.size(); i++) {
					cout << "[" << i + 1 << "] MultiBank_ATM : " << ((MultiBank_ATM_list[i]->Call_language_type() == 0) ? ("Unilingual type , ") : ("Bilingual type , "))
						<< "Primary bank name : " << MultiBank_ATM_list[i]->Call_primary_bank_name() << " , Serial number " << MultiBank_ATM_list[i]->Call_serial_number() << endl;
				}
				int foo;
				foo = Cin<int>(1, 0);

				if (foo > (int)MultiBank_ATM_list.size() || foo <= 0) {
					cout << "ERROR:Please write valid number" << endl;
					continue;
				}
				MultiBank_ATM_list[foo - 1]->Start_ATM();

			}
			else {
				cout << "ERROR:Please write valid number!" << endl;
				continue;
			}
		}
		catch (bad_alloc& e) {
			cout << e.what() << endl;
			cout << "ERROR:Please write valid number" << endl;
			continue;
		}
		catch (int _void) {
			cout << "ERROR:Please write valid number" << endl;
			continue;
		}
	}
}


int main() {

	Construct_part();
	Execution_part();
	return 0;
}
