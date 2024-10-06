#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

struct stClientData
{
	string AccountNum = "";
	string PinCode = "";
	string FullName = "";
	string PhoneNum = "";
	double AccountBalance = 0;
	bool MarkedToDelete = false;
};

struct stUserInfo
{
	string UserName = "";
	string Password = "";
	int Permissions = 0;
	bool MarkedToDelete = false;
};

enum enMainMenuOptions
{
	ShowClientsData = 1, AddNewClient = 2,
	DeleteClient = 3, UpdateClientInfo = 4,
	FindClient = 5, TransactionSection = 6, ManageUsers = 7, Logout = 8
};

enum enTransactionMenuOptions {
	Deposit = 1, Withdraw = 2,
	TotalBalances = 3, ToMainMenu = 4
};

enum enManageUsersMenuOptions {
	UsersList =1, AddNewUser =2, DeleteUser=3, UpdateUser=4, FindUser =5, BackMainMenu=6
};

enum enPermissions {
	prAll = -1,  prShowClientsData = 1, prAddNewClient = 2,
	prDeleteClient = 4, prUpdateClientInfo = 8,
	prFindClient = 16, prTransactionSection = 32, prManageUsers = 64
};

//Global Variables
const string ClientsFileName = "ClientsData.txt";
const string UsersFileName = "Users.txt";
stUserInfo CurrentUser;

void TransactionScreen();
void MainMenu();
bool isClientExists(vector <stClientData>& vClients, string AccountNum);
void Login();
void ManageUsersScreen();


short ReadNumInRange(string Message, short From, short To)
{
	short Num = 0;
	do {
		cout << Message;

		if (cin >> Num)
		{
			if (Num <From || Num > To)
				continue;
			else
				return Num;
		}
		else
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

	} while (Num < From || Num > To);
	
}

string ReadAccountNumber(vector <stClientData> vClients)
{
	string AccountNum;
	cout << "\nPlease Enter account number : ";
	getline(cin >> ws, AccountNum);
	while(!isClientExists(vClients, AccountNum))
	{
		cout << "\nClient with account number [" << AccountNum << "] Does not exist.\n\nPlease, Enter Account Number : ";
		getline(cin >> ws, AccountNum);
	}
	return AccountNum;
}

void BackToMainMenu()
{
	cout << "\nPress any key to back to the Main Menu . . .";
	system("pause>0");
	MainMenu();
}

void AccessDenied()
{
	system("cls");
	cout << "_____________________________________\n\n";
	cout << "Access Denied,\n";
	cout << "You don't have permission to do this,\n";
	cout << "Please Contect your admin\n";
	cout << "_____________________________________\n";
	BackToMainMenu();
}

void PrintMainMenuTable()
{
	cout << "=================================================\n";
	cout << "               Main Menu Screen                  \n";
	cout << "=================================================\n";
	cout << "\t[1] Show Clients Lists.\n";
	cout << "\t[2] Add New Client.\n";
	cout << "\t[3] Delete Client.\n";
	cout << "\t[4] Update Client Info.\n";
	cout << "\t[5] Find Client.\n";
	cout << "\t[6] Transactions.\n";
	cout << "\t[7] Manage Users.\n";
	cout << "\t[8] Logout.\n";
	cout << "=================================================\n";
}

vector <string> SplitLine(string Line, string Delim)
{
	vector <string> vWords;
	short DeliPos=0;
	string Word = "";
	while ((DeliPos = Line.find(Delim)) != string::npos)
	{
		Word = Line.substr(0, DeliPos);
		if (Word != "")
			vWords.push_back(Word);
		Line.erase(0, DeliPos + Delim.length());
	}
	if (Line != "")
		vWords.push_back(Line);
	return vWords;
}

stClientData ConvertClientLineToRecord(string Line, string Delim = "#//#")
{
	vector <string> vLineInfo = SplitLine(Line, Delim);
	stClientData Client;
	Client.AccountNum = vLineInfo[0];
	Client.PinCode = vLineInfo[1];
	Client.FullName = vLineInfo[2];
	Client.PhoneNum = vLineInfo[3];
	Client.AccountBalance = stod(vLineInfo[4]);
	return Client;
}

vector <stClientData> LoadClientsDataFromFile(string FileName)
{
	vector <stClientData> vClients;
	fstream File;
	File.open(FileName, ios::in);//Read Mode

	if (File.is_open())
	{
		string Line = "";
		while (getline(File, Line))
		{
			if(Line != "")
				vClients.push_back(ConvertClientLineToRecord(Line));
		}
		File.close();
	}
	return vClients;
}

stUserInfo ConvertUserLineToRecord(string Line, string Delim = "#//#")
{
	vector <string> vLineInfo = SplitLine(Line, Delim);
	stUserInfo User;
	User.UserName = vLineInfo[0];
	User.Password = vLineInfo[1];
	User.Permissions = stoi(vLineInfo[2]);
	return User;
}

vector <stUserInfo> LoadUsersDataFromFile(string FileName)
{
	vector <stUserInfo> vUsers;
	fstream File;
	File.open(FileName, ios::in);//Read Mode

	if (File.is_open())
	{
		string Line = "";
		while (getline(File, Line))
		{
			if (Line != "")
				vUsers.push_back(ConvertUserLineToRecord(Line));
		}
		File.close();
	}
	return vUsers;
}

bool FindUserByUserNameAndPassword(string UserName, string Password, stUserInfo & enteredUser)
{
	vector <stUserInfo> Users = LoadUsersDataFromFile(UsersFileName);
	for (stUserInfo User : Users)
	{
		if (User.UserName == UserName && User.Password == Password)
		{
			enteredUser = User;
			return true;
		}
	}
	return false;
}

bool isUserExist(string UserName, string Password, stUserInfo& User)
{
	if(FindUserByUserNameAndPassword(UserName, Password, User))
		return true;
	else
		return false;
		
}

void PrintHeaderTable(int ClientsNum)
{

	cout << "\t\t\t\tClient List (" << ClientsNum << ") Client(s)\n";
	cout << "______________________________________________________";
	cout << "_________________________________________________________\n\n";
	cout << left;
	cout << "| " << setw(20) << "Account Number";
	cout << "| " << setw(10) << "Pin Code";
	cout << "| " << setw(40) << "Client Name ";
	cout << "| " << setw(15) << "Phone Number";
	cout << "| " << setw(15) << "Balance";
	cout << "\n______________________________________________________";
	cout << "_________________________________________________________\n\n";
}

void PrintClientInTable(stClientData Client)
{
	cout << "| " << setw(20) << Client.AccountNum;
	cout << "| " << setw(10) << Client.PinCode;
	cout << "| " << setw(40) << Client.FullName;
	cout << "| " << setw(15) << Client.PhoneNum;
	cout << "| " << setw(15) << Client.AccountBalance;
}

void PrintBodyTable(vector <stClientData> &vClients)
{
	for (stClientData& Client : vClients)
	{
		PrintClientInTable(Client);
		cout << endl;
	}
}

void PrintFooterTable()
{
	cout << "\n______________________________________________________";
	cout << "_________________________________________________________\n";
}

bool CheckUserPermission(enPermissions Permission)
{
	if (CurrentUser.Permissions == enPermissions::prAll)
		return true;
	else if (CurrentUser.Permissions & Permission)
		return true;
	else
		return false;
}

void ShowClientsDataScreen(vector <stClientData> &vClients)
{
	if (!CheckUserPermission(enPermissions::prShowClientsData))
	{
		AccessDenied();
	}

	system("cls");
	PrintHeaderTable(vClients.size());
	PrintBodyTable(vClients);
	PrintFooterTable();

	BackToMainMenu();
}

bool isClientExists(vector <stClientData>& vClients, string AccountNum)
{
	for (stClientData& C : vClients)
	{
		if (C.AccountNum == AccountNum)
			return true;
	}
	return false;
}

void ReadNewClient(vector <stClientData>& vClients)
{
	stClientData NewClient;
	bool AccountNumExists = false;
	
	cout << "\nEnter Account Number : ";
	getline(cin >> ws, NewClient.AccountNum);

	while (isClientExists(vClients, NewClient.AccountNum))
	{
		cout << "\nClient with [" << NewClient.AccountNum << "] already exists, Enter another Account Number : ";
		getline(cin, NewClient.AccountNum);
	}

	cout << "Enter Pin Code : ";
	getline(cin, NewClient.PinCode);
	cout << "Enter Full Name : ";
	getline(cin, NewClient.FullName);
	cout << "Enter Phone Number : ";
	getline(cin, NewClient.PhoneNum);
	cout << "Enter Account Balance : ";
	cin >> NewClient.AccountBalance;
	vClients.push_back(NewClient);	
}

string ConvertClientRecordToLine(stClientData Client, string Separator = "#//#")
{
	string Text = "";
	Text += Client.AccountNum + Separator;
	Text += Client.PinCode + Separator;
	Text += Client.FullName + Separator;
	Text += Client.PhoneNum + Separator;
	Text += to_string(Client.AccountBalance);
	return Text;
}

void SaveClientVectorToFile(string FileName, vector <stClientData> &vClients)
{
	fstream File;
	File.open(FileName, ios::out);
	if (File.is_open())
	{
		for (stClientData& C : vClients)
		{
			if (C.MarkedToDelete == false)
				File << ConvertClientRecordToLine(C) << endl;
		}
		File.close();
	}

}

void AddNewClientScreen(vector <stClientData> &vClients)
{
	if (!CheckUserPermission(enPermissions::prAddNewClient))
	{
		AccessDenied();
	}

	system("cls");
	char AddMore = 'n';
	cout << "_____________________________________\n\n";
	cout << "\tAdd New Clients Screen\n";
	cout << "_____________________________________\n";
	do
	{
		ReadNewClient(vClients);

		cout << "\nDo you want to add more clients y/n : ";
		cin >> AddMore;
	} while (tolower(AddMore) == 'y');

	// Now i will creat a function to save the Data to the file so the new clients will be added
	SaveClientVectorToFile(ClientsFileName, vClients);

	BackToMainMenu();
}

void PrintClientCard(stClientData Client)
{
	cout << "\n\tClient Details :\n";
	cout << "_____________________________________\n";
	cout << "\nAccount Number  : " << Client.AccountNum;
	cout << "\nPin Code        : " << Client.PinCode;
	cout << "\nFull Name       : " << Client.FullName;
	cout << "\nPhone Number    : " << Client.PhoneNum;
	cout << "\nAccount Balance : " << Client.AccountBalance;
	cout << "\n_____________________________________\n";

	cout << endl;
}

void FindAndPrintCientCard(vector <stClientData>& vClients, string AccountNum)
{
	for (stClientData& C : vClients)
	{
		if (C.AccountNum == AccountNum)
		{
			PrintClientCard(C);
			break;
		}
	}
}

void MarkClientToDelete(vector <stClientData>& vClients, string AccountNum)
{
	for (stClientData& C : vClients)
	{
		if (C.AccountNum == AccountNum)
		{
			C.MarkedToDelete = true;
			break;
		}
	}
}

void DeleteClientScreen(vector <stClientData> &vClients)
{
	if (!CheckUserPermission(enPermissions::prDeleteClient))
	{
		AccessDenied();
	}

	system("cls");
	string AccountNumToDelete;
	char Answer = 'n';
	cout << "_____________________________________\n\n";
	cout << "\tDelete Client Screen\n";
	cout << "_____________________________________\n";
	cout << "\nPlease, Enter account number : ";
	getline(cin >> ws, AccountNumToDelete);

	if (isClientExists(vClients, AccountNumToDelete))
	{
		FindAndPrintCientCard(vClients, AccountNumToDelete);
		cout << "\nAre you sure you want to delete this client y/n : ";
		cin >> Answer;
		if (tolower(Answer) == 'y')
		{
			MarkClientToDelete(vClients, AccountNumToDelete);
			SaveClientVectorToFile(ClientsFileName, vClients);
			cout << "\nClient Deleted Successfully.\n";
		}
	}
	else
	{
		cout << "\nClient With account number (" << AccountNumToDelete << ") is Not Found !\n";
	}
	BackToMainMenu();
}

void ReadNewClientInfo(vector <stClientData>& vClients, string AccountNum)
{
	for (stClientData& C : vClients)
	{
		if (C.AccountNum == AccountNum)
		{
			cout << "\nEnter Pin Code        : ";
			getline(cin >> ws, C.PinCode);
			cout << "\nEnter Full Name       : ";
			getline(cin, C.FullName);
			cout << "\nEnter Phone Number    : ";
			getline(cin, C.PhoneNum);
			cout << "\nEnter Account Balance : ";
			cin >> C.AccountBalance;
		}
	}
}

void UpdateClientInfoScreen(vector <stClientData>& vClients)
{
	if (!CheckUserPermission(enPermissions::prUpdateClientInfo))
	{
		AccessDenied();
	}

	system("cls");
	string AccountNum;
	char Answer = 'n';
	cout << "_____________________________________\n\n";
	cout << "\tUpdate Client Info Screen\n";
	cout << "_____________________________________\n";
	cout << "\nPlease, Enter Account Number : ";
	getline(cin >> ws, AccountNum);

	if (isClientExists(vClients, AccountNum))
	{
		FindAndPrintCientCard(vClients, AccountNum);
		cout << "\nAre you sure you to update this client's info : ";
		cin >> Answer;
		if (tolower(Answer) == 'y')
		{
			ReadNewClientInfo(vClients, AccountNum);
			SaveClientVectorToFile(ClientsFileName, vClients);
			cout << "\nClient's Info Updated Successfully.\n";
		}
	}
	else
	{
		cout << "\nThe Client With account number [" << AccountNum << "] is not found!\n";
	}

	BackToMainMenu();
}

void FindClientScreen(vector <stClientData>& vClients)
{
	if (!CheckUserPermission(enPermissions::prFindClient))
	{
		AccessDenied();
	}

	system("cls");
	cout << "_____________________________________\n\n";
	cout << "\tFind Client Screen\n";
	cout << "_____________________________________\n";
	string AccountNum;
	cout << "\nPlease, Enter account number : ";
	getline(cin>> ws, AccountNum);
	if (isClientExists(vClients, AccountNum))
	{
		FindAndPrintCientCard(vClients, AccountNum);
	}
	else
	{
		cout << "\nClient with account number [" << AccountNum << "] is Not Found !\n";
	}

	BackToMainMenu();
}

void PrintTransactionScreen()
{
	cout << "=================================================\n";
	cout << "            Transaction Menu Screen              \n";
	cout << "=================================================\n";
	cout << "\t[1] Deposit.\n";
	cout << "\t[2] Withdraw.\n";
	cout << "\t[3] Total Balances.\n";
	cout << "\t[4] Main Menu.\n";
	cout << "=================================================\n";

}

void BackToTransactionMenu()
{
	cout << "\n\nPress Any key to back to Transaction Menu . . .";
	system("pause>0");
	TransactionScreen();
}

double DepositMoneyByAccountNum(vector <stClientData>& vClients, string AccountNum, double DepositAmount)
{
	double NewBalance = 0;
	for (stClientData& C : vClients)
	{
		if (C.AccountNum == AccountNum)
		{
			C.AccountBalance += DepositAmount;
			NewBalance = C.AccountBalance;
			break;
		}
	}
	return NewBalance;
}

void DepositScreen(vector <stClientData> &vClients)
{
	cout << "_____________________________________\n\n";
	cout << "\tDeposit Screen\n";
	cout << "_____________________________________\n";

	double DepositAmount = 0;
	string AccountNum;
	AccountNum = ReadAccountNumber(vClients);
	FindAndPrintCientCard(vClients, AccountNum);

	cout << "\nPlease, Enter deposit amount : ";
	cin >> DepositAmount;
	char Answer = 'n';
	cout << "\nAre you sure you want to perform this transaction y/n : ";
	cin >> Answer;
	
	if (tolower(Answer) == 'y')
	{
		cout << "\nDone Successfully, The New Balance = " << DepositMoneyByAccountNum(vClients, AccountNum, DepositAmount);
		SaveClientVectorToFile(ClientsFileName, vClients);
	}	
}

double ReadWithdrawAmount(vector <stClientData>& vClients, string AccountNum)
{
	stClientData Client;
	double WithdrawAmount = 0;
	for (stClientData& C : vClients)
	{
		if (C.AccountNum == AccountNum)
		{
			Client = C;
			break;
		}
	}
	cout << "\nPlease, Enter withdraw amount : ";
	cin >> WithdrawAmount;
	while (WithdrawAmount > Client.AccountBalance)
	{
		cout << "\nAmount exceeds the balance. You can only withdraw up to : " << Client.AccountBalance;
		cout << "\nPlease, Enter withdraw amount : ";
		cin >> WithdrawAmount;
	}
	return WithdrawAmount;
}

void WithdrawScreen(vector <stClientData>& vClients)
{

	cout << "_____________________________________\n\n";
	cout << "\tWithdraw Screen\n";
	cout << "_____________________________________\n";

	double WithdrawAmount = 0;
	string AccountNum;
	AccountNum = ReadAccountNumber(vClients);

	FindAndPrintCientCard(vClients, AccountNum);

	WithdrawAmount = ReadWithdrawAmount(vClients, AccountNum);

	char Answer = 'n';
	cout << "\nAre you sure you want to perform this transaction y/n : ";
	cin >> Answer;

	if (tolower(Answer) == 'y')
	{
		cout << "\nDone Successfully, The New Balance = " << DepositMoneyByAccountNum(vClients, AccountNum, WithdrawAmount* (-1));
		SaveClientVectorToFile(ClientsFileName, vClients);
	}
}

void TotalBalancesHeader(int ClientsNum)
{
	cout << "\t\t\t\tClient List (" << ClientsNum << ") Client(s)\n";
	cout << "______________________________________________________";
	cout << "_________________________________________________________\n\n";
	cout << left;
	cout << "| " << setw(20) << "Account Number";
	cout << "| " << setw(40) << "Client Name ";
	cout << "| " << setw(15) << "Balance";
	cout << "\n______________________________________________________";
	cout << "_________________________________________________________\n\n";
}

void PrintShortClientInfo(stClientData Client)
{
	cout << "| " << setw(20) << Client.AccountNum;
	cout << "| " << setw(40) << Client.FullName;
	cout << "| " << setw(15) << Client.AccountBalance;
}

void TotalBalancesBody(vector <stClientData>& vClients)
{
	for (stClientData& C : vClients)
	{
		PrintShortClientInfo(C);
		cout << endl;
	}
}

double GetTotalBalances(vector <stClientData>& vClients)
{
	double TotalBalances = 0;
	for (stClientData& C : vClients)
	{
		TotalBalances += C.AccountBalance;
	}
	return TotalBalances;
}

void TotalBalancesScreen(vector <stClientData>& vClients)
{
	TotalBalancesHeader(vClients.size());
	TotalBalancesBody(vClients);
	PrintFooterTable();

	cout << "\n\n\t\t\t\t\tTotal Balances = " << GetTotalBalances(vClients) << endl;
	
}

void TransactionScreen()
{

	if (!CheckUserPermission(enPermissions::prTransactionSection))
	{
		AccessDenied();
	}

	system("cls");
	PrintTransactionScreen();

	vector <stClientData> vClients = LoadClientsDataFromFile(ClientsFileName);
	enTransactionMenuOptions UserChoice;
	UserChoice = (enTransactionMenuOptions)ReadNumInRange("Choose what do you want to do [1 to 4] : ", 1, 4);

	switch (UserChoice)
	{
	case enTransactionMenuOptions::Deposit:
		system("cls");
		DepositScreen(vClients);
		BackToTransactionMenu();
	
	case enTransactionMenuOptions::Withdraw:
		system("cls");
		WithdrawScreen(vClients);
		BackToTransactionMenu();

	case enTransactionMenuOptions::TotalBalances:
		system("cls");
		TotalBalancesScreen(vClients);
		BackToTransactionMenu();

	case enTransactionMenuOptions::ToMainMenu:
		MainMenu();
	}

		
	
}

void PrintManageUsersMenuTable()
{
	cout << "=================================================\n";
	cout << "            Manage Users Menu Screen             \n";
	cout << "=================================================\n";
	cout << "\t[1] Users List.\n";
	cout << "\t[2] Add New User.\n";
	cout << "\t[3] Delete User.\n";
	cout << "\t[4] Update User.\n";
	cout << "\t[5] Find User.\n";
	cout << "\t[6] MainMenu.\n";
	cout << "=================================================\n";
}

void PrintUsersListHeader(int UsersNum)
{
	cout << "\t\t\t\tClient List (" << UsersNum << ") Client(s)\n";
	cout << "______________________________________________________";
	cout << "_________________________________________________________\n\n";
	cout << left;
	cout << "| " << setw(20) << "User Name";
	cout << "| " << setw(40) << "Password ";
	cout << "| " << setw(15) << "Permissions";
	cout << "\n______________________________________________________";
	cout << "_________________________________________________________\n\n";

}

void PrintUserRow(stUserInfo User)
{
	cout << "| " << setw(20) << User.UserName;
	cout << "| " << setw(40) << User.Password;
	cout << "| " << setw(15) << User.Permissions;
}

void PrintUsersListBody(vector <stUserInfo> Users)
{
	for (stUserInfo User : Users)
	{
		PrintUserRow(User);
		cout << endl;
	}
}

void BackToManageUsersScreen()
{
	cout << "\nPress Any key to back to Manage Users Screen . . .";
	system("pause >0");
	system("cls");
	ManageUsersScreen();
}

void UsersListScreen()
{
	vector <stUserInfo> Users = LoadUsersDataFromFile(UsersFileName);
	PrintUsersListHeader(Users.size());
	PrintUsersListBody(Users);
	PrintFooterTable();
	BackToManageUsersScreen();
}

bool isUserNameExist(string UserName , stUserInfo &GivenUser)
{
	vector <stUserInfo> Users = LoadUsersDataFromFile(UsersFileName);
	for (stUserInfo User : Users)
	{
		if (User.UserName == UserName)
		{
			GivenUser = User;
			return true;
		}
	}
	return false;
}

int ReadUserPermissions()
{
	int UserPer = 0;
	char Answer = 'n';

	cout << "\nDo you want to give this user full access y/n : ";
	cin >> Answer;

	if (tolower(Answer) == 'y')
		return  -1;

	cout << "\nWhat permissins do you want to give to this user :\n";

	cout << "\nShow Client List y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prShowClientsData;

	cout << "\nAdd New Client y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prAddNewClient;

	cout << "\nDelete Client y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prDeleteClient;

	cout << "\nUpdate Client Info y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prUpdateClientInfo;

	cout << "\nFind Client y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prFindClient;

	cout << "\nTransactions Screen y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prTransactionSection;

	cout << "\nManage Users Screen y/n : ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
		UserPer += (int)enPermissions::prManageUsers;

	return UserPer;
}

stUserInfo ReadNewUser()
{
	stUserInfo NewUser;
	char GiveFullAccess = 'n';

	cout << "\nEnter User Name : ";
	getline(cin >> ws, NewUser.UserName);
	while (isUserNameExist(NewUser.UserName,NewUser))
	{
		cout << "\nThe UserName [" << NewUser.UserName << "] is already taken, Enter another UserName : ";
		getline(cin >> ws, NewUser.UserName);
	}
	cout << "\nEnter Password : ";
	getline(cin, NewUser.Password);

	NewUser.Permissions = ReadUserPermissions();


	return NewUser;
}

string ConvertUserRecordToLine(stUserInfo User, string Separator = "#//#")
{
	string Text = "";
	Text += User.UserName + Separator;
	Text += User.Password + Separator;
	Text += to_string(User.Permissions) + Separator;
	return Text;
}

void SaveUsersVectorToFile(vector <stUserInfo> vUsers, string FileName)
{
	fstream File;
	File.open(FileName, ios::out);
	if (File.is_open())
	{
		for (stUserInfo User : vUsers)
		{
			if(!User.MarkedToDelete)
				File << ConvertUserRecordToLine(User) << endl;
		}
		File.close();
	}
}

void AddNewUserScreen()
{
	vector <stUserInfo> Users = LoadUsersDataFromFile(UsersFileName);

	cout << "_____________________________________\n\n";
	cout << "\tAdd New User Screen\n";
	cout << "_____________________________________\n";
	char AddMore = 'n';
	do
	{
		stUserInfo NewUser = ReadNewUser();
		Users.push_back(NewUser);
		//save the vector after I push back the new user in it.
		SaveUsersVectorToFile(Users, UsersFileName);

		cout << "\nDo you want to add more users y/n : ";
		cin >> AddMore;

	} while (AddMore == 'y' || AddMore == 'Y');

	BackToManageUsersScreen();
}

void PrintUserCard(stUserInfo User)
{
		cout << "\n\tUser Details :\n";
		cout << "_____________________________________\n";
		cout << "\nUser Name   : " << User.UserName;
		cout << "\nPassword    : " << User.Password;
		cout << "\nPermissions : " << User.Permissions;
		cout << "\n_____________________________________\n";

		cout << endl;
}

void MarkUserToDelete(vector <stUserInfo> &vUsers, string UserName)
{
	for (stUserInfo& User : vUsers)
	{
		if (User.UserName == UserName)
		{
			User.MarkedToDelete = true;
		}
	}
}

void DeleteUserScreen()
{
	vector <stUserInfo> vUsers = LoadUsersDataFromFile(UsersFileName);
	stUserInfo delUser;
	char UserAnswer = 'n';

	cout << "_____________________________________\n\n";
	cout << "\tDelete User Screen\n";
	cout << "_____________________________________\n";

	cout << "\nEnter User Name : ";
	getline(cin >> ws, delUser.UserName);

	if(delUser.UserName == "Admin")
	{
		cout << "\nYou can't Delete The Admin!\n";
		BackToManageUsersScreen();
	}
	 
	if(isUserNameExist(delUser.UserName, delUser))
	{
		PrintUserCard(delUser);
		cout << "\nAre you sure you want to delete this user y/n : ";
		cin >> UserAnswer;
		if (tolower(UserAnswer) == 'y')
		{
			//Make a function to Mark user to delete
			MarkUserToDelete(vUsers, delUser.UserName);
			SaveUsersVectorToFile(vUsers, UsersFileName);
			cout << "\nThe User Has been deleted successfully.\n";

		}
	}
	else
	{
		cout << "\nUser with [" << delUser.UserName << " is not found !!";
	}


	BackToManageUsersScreen();
}

void UpdateUserInfo(vector <stUserInfo>& vUsers, string UserName)
{
	for (stUserInfo& User : vUsers)
	{
		if (User.UserName == UserName)
		{
			cout << "Enter Password : ";
			getline(cin >>ws, User.Password);
			User.Permissions = ReadUserPermissions();
			break;
		}
	}

}

void UpdateUserScreen()
{
	vector <stUserInfo> vUsers = LoadUsersDataFromFile(UsersFileName);
	stUserInfo UpdUser;
	char UserAnswer = 'n';

	cout << "_____________________________________\n\n";
	cout << "\tUpdate User Screen\n";
	cout << "_____________________________________\n";

	cout << "\nEnter User Name : ";
	getline(cin >> ws, UpdUser.UserName);

	if(UpdUser.UserName == "Admin")
	{
		cout << "\nYou cannot Update The Admin Info.\n";
		BackToManageUsersScreen();
	}

	if (isUserNameExist(UpdUser.UserName, UpdUser))
	{
		PrintUserCard(UpdUser);
		cout << "\nAre you sure you want to update this user's info : ";
		cin >> UserAnswer;
		if (tolower(UserAnswer) == 'y')
		{
			UpdateUserInfo(vUsers, UpdUser.UserName);
			SaveUsersVectorToFile(vUsers, UsersFileName);
			cout << "\nUser's information has been updated successfully.\n";
		}	
	}
	else
	{
		cout << "\nUser with [" << UpdUser.UserName << " is not found !!";
	}
	BackToManageUsersScreen();
}

void FindUserScreen()
{
	stUserInfo User;
	cout << "_____________________________________\n\n";
	cout << "\tFind User Screen\n";
	cout << "_____________________________________\n";

	cout << "\nEnter User Name : ";
	getline(cin >> ws, User.UserName);

	if (isUserNameExist(User.UserName, User))
	{
		PrintUserCard(User);
	}
	else
	{
		cout << "\nThis user is not found!\n";
	}
	BackToManageUsersScreen();
	
}

void PerformManageScreenOptions(enManageUsersMenuOptions Option)
{
	switch (Option)
	{
	case enManageUsersMenuOptions::UsersList:
		system("cls");
		UsersListScreen();
		break;

	case enManageUsersMenuOptions::AddNewUser:
		system("cls");
		AddNewUserScreen();
		break;


	case enManageUsersMenuOptions::DeleteUser:
		system("cls");
		DeleteUserScreen();
		break;

	case enManageUsersMenuOptions::UpdateUser:
		system("cls");
		UpdateUserScreen();
		break;

	case enManageUsersMenuOptions::FindUser:
		system("cls");
		FindUserScreen();
		break;

	case enManageUsersMenuOptions::BackMainMenu:
		system("cls");
		MainMenu();
	}
}

void ManageUsersScreen()
{
	if (!CheckUserPermission(enPermissions::prManageUsers))
	{
		AccessDenied();
	}
	system("cls");
	PrintManageUsersMenuTable();
	enManageUsersMenuOptions UserOption = (enManageUsersMenuOptions)ReadNumInRange("Choose what do you what to do[1 to 6] : ", 1, 6);
	PerformManageScreenOptions(UserOption);
}

void PerformMainMenuOption(enMainMenuOptions UserChoice)
{
	vector <stClientData> vClients = LoadClientsDataFromFile(ClientsFileName);

	switch (UserChoice)
	{
	case enMainMenuOptions::ShowClientsData:
		ShowClientsDataScreen(vClients);
		break;

	case enMainMenuOptions::AddNewClient:
		AddNewClientScreen(vClients);
		break;
	case enMainMenuOptions::DeleteClient:
		DeleteClientScreen(vClients);
		break;
	case enMainMenuOptions::UpdateClientInfo:
			UpdateClientInfoScreen(vClients);
		break;

	case enMainMenuOptions::FindClient:
		FindClientScreen(vClients);
		break;

	case enMainMenuOptions::TransactionSection:
		TransactionScreen();
		break;

	case enMainMenuOptions::ManageUsers:
		ManageUsersScreen();
		break;

	case enMainMenuOptions::Logout:
		Login();
		break;

	default:
		Login();
	}
}

void MainMenu()
{
	system("cls");
	enMainMenuOptions UserChoice;
	PrintMainMenuTable();

	UserChoice = (enMainMenuOptions)ReadNumInRange("Choose what do you want to do [1 to 7] : ", 1, 8);
	PerformMainMenuOption(UserChoice);
}

void Login()
{
	string UserName;
	string Password;
	bool LoginFailed = false;

	do
	{
		system("cls");
		cout << "_____________________________________\n\n";
		cout << "\tLogin Screen\n";
		cout << "_____________________________________\n";
		if(LoginFailed)
			cout <<  "\nInvalid UserName/Password!!\n";

		cout << "\nPlease, Enter User Name : ";
		getline(cin >> ws, UserName);
		cout << "\nPlease, Enter Password  : ";
		getline(cin, Password);

		LoginFailed = !isUserExist(UserName, Password, CurrentUser);

	} while (LoginFailed);

	MainMenu();
}

int main()
{
	Login();
	system("pause>0");
	return 0;
}