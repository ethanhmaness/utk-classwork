// lab5.cpp
// This lab simulates a shopping cart that can hold items
// Ethan H. Maness
// 11/13/20

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <stdexcept>

using namespace std;

char PrintMenu();

//Item class
class Item {

    string name;
    string description;
    double price;
    int quantity;
    
    public:
        //constructor
        Item(string n = "none", string d = "none", double p = 0, int q = 0) : name(n), description(d), price(p), quantity(q) {}

        //mutators
        void SetName(string n){ name = n; }
        void SetDescription(string d){ description = d; }
        void SetPrice(double p){ price = p; }
        void SetQuantity(int q){ quantity = q; }

        //accessors
        string GetName() const { return name; }
        string GetDescription() const { return description; }
        double GetPrice() const { return price; }
        int GetQuantity() const { return quantity; }
        void Print() const { cout << name << ": " << quantity << " at $" << fixed << setprecision(2) << price << " = $" << (quantity * price) << "\n"; }
        void PrintDescription() const { cout << name << ": " << description << "\n"; }

};

//ShoppingCart class
class ShoppingCart {

    string name;
    string date;
    vector<Item> cartItems;

    public:
        
        //constructor
        ShoppingCart(string n = "None", string d = "April 1, 2020") : name(n), date(d) {}

        //mutators
        void AddItem(const Item& i){ cartItems.push_back(i); }
        void RemoveItem(const string&);
        void ChangeQuantity(const string&, int);

        //accessors
        string GetCustomerName() const { return name; }
        string GetDate() const { return date; }
        int FindItemIndex(const string&) const;
        int GetTotalQuantity() const;
        double GetTotalCost() const;
        void PrintTotal() const;
        void PrintDescriptions() const;

};


int main(){

    //init name and date string
    string custName;
    string date;

    //get name and date from input and make ShoppingCart accordingly
    cout << "Enter customer name: ";
    getline(cin, custName);
    cout << "Enter today's date: ";
    getline(cin, date);

    ShoppingCart cart(custName, date);

    //print values back out
    cout << "\n";
    cout << "Customer's Name: " << cart.GetCustomerName() << "\n";
    cout << "Today's Date: " << cart.GetDate() << "\n";

    //temporary utility vars
    Item tempItem;
    string tempString;
    string tempName;
    string tempDescription;
    double tempPrice;
    int tempQuantity;
    bool rerun;

    //main loop, continually prompts for option and acts accordingly until user quits
    char option;
    do{

        //print menu
        cout << "\n";

        option = PrintMenu();

        //do the thing
        switch(option){

            //Add item to cart
            case 'a':

                //header
                cout << "ADD ITEM TO CART" << "\n";

                //get name
                cout << "Enter the item name: ";
                getline(cin, tempName);

                //get description
                cout << "Enter the item description: ";
                getline(cin, tempDescription);

                //get price, redo if not double
                do{
                    cout << "Enter the item price: $";
                    try{
                        //start with flag false
                        rerun = false;
                        //get string, convert to double
                        getline(cin, tempString);
                        tempPrice = stod(tempString);
                    }
                    //if error, set flag to true so loop runs again
                    catch(invalid_argument &){
                        cout << "Invalid input\n";
                        rerun = true;
                    }
                    catch(out_of_range &){
                        cout << "Invalid input\n";
                        rerun = true;
                    }
                } while(rerun);

                //get quantity, redo if not integer
                do{
                    cout << "Enter the item quantity: ";
                    try{
                        //start with flag false
                        rerun = false;
                        //read string, convert to int
                        getline(cin, tempString);
                        tempQuantity = stoi(tempString);
                    }
                    //if error, flag true so loop runs again
                    catch(invalid_argument &){
                        cout << "Invalid input\n";
                        rerun = true;
                    }
                    catch(out_of_range &){
                        cout << "Invalid input\n";
                        rerun = true;
                    }
                } while(rerun);

                //set tempItem to inputted values
                tempItem.SetName(tempName);
                tempItem.SetDescription(tempDescription);
                tempItem.SetPrice(tempPrice);
                tempItem.SetQuantity(tempQuantity);

                //add tempItem into ShoppingCart's item vector
                cart.AddItem(tempItem);

            break;

            //remove item from cart
            case 'd':

                //get name
                cout << "REMOVE ITEM FROM CART" << "\n";
                cout << "Enter the item name to remove: ";
                getline(cin, tempName);

                //remove
                cart.RemoveItem(tempName);

            break;

            //change item quantity
            case 'c':

                //get name
                cout << "CHANGE ITEM QUANTITY" << "\n";
                cout << "Enter the item name: ";
                getline(cin, tempName);

                //get quantity, redo if not integer
                do{
                    cout << "Enter the new quantity: ";
                    try{
                        //start with flag false
                        rerun = false;
                        //read string, convert to int
                        getline(cin, tempString);
                        tempQuantity = stoi(tempString);
                    }
                    //if error, flag true so loop runs again
                    catch(invalid_argument &){
                        cout << "Invalid input\n";
                        rerun = true;
                    }
                    catch(out_of_range &){
                        cout << "Invalid input\n";
                        rerun = true;
                    }
                } while(rerun);

                //remove if new quantity is 0, otherwise change quantity
                if(tempQuantity == 0) cart.RemoveItem(tempName);
                else cart.ChangeQuantity(tempName, tempQuantity);

            break;

            //print descriptions of items
            case 'i':

                cart.PrintDescriptions();

            break;

            //print price totals
            case 'o':

                cart.PrintTotal();

            break;

        }

    } while(option != 'q');

    return 0;

}


//prints menu, prompts user for a character
char PrintMenu(){

    char c;
    string tempString;

    //menu
    cout << "MENU" << "\n";
    cout << "a - Add item to cart" << "\n";
    cout << "d - Remove item from cart" << "\n";
    cout << "c - Change item quantity" << "\n";
    cout << "i - Output item descriptions" << "\n";
    cout << "o - Output shopping cart" << "\n";
    cout << "q - Quit" << "\n";
    cout << "\n";
    cout << "Choose an option: ";

    //take first char from line as the select char
    getline(cin, tempString);
    c = tempString.at(0);

    cout << "\n";

    return c;

}

//finds first index of an item in cartItems with a name string equal to the parameter
int ShoppingCart::FindItemIndex(const string& s) const {

    for(unsigned int i = 0; i < cartItems.size(); i++){

        if(cartItems.at(i).GetName() == s) return i;

    }

    //sentinel if item not found
    return -1;

}

//removes an item from cartItems based on name
void ShoppingCart::RemoveItem(const string& s){

    int i = FindItemIndex(s);

    if(i != -1) cartItems.erase(cartItems.begin() + i);

    else cout << "Item not found in cart.";

}

//changes quantity of item in cartItems based on name
void ShoppingCart::ChangeQuantity(const string& s, int q){

    int i = FindItemIndex(s);

    if(i != -1) cartItems.at(i).SetQuantity(q);

    else cout << "Item not found in cart.";

    cout << "\n";

}

//returns total item count
int ShoppingCart::GetTotalQuantity() const {

    int sum = 0;
    for(Item item : cartItems) sum += item.GetQuantity();
    return sum;

}

//returns total cost of all items in cart
double ShoppingCart::GetTotalCost() const {

    double sum = 0;
    for(Item item : cartItems) sum += (item.GetPrice() * item.GetQuantity());
    return sum;

}

//prints all items in shopping cart, followed by the total cost
void ShoppingCart::PrintTotal() const {

    cout << name << "'s Shopping Cart - " << date << "\n";
    cout << "Number of Items: " << cartItems.size() << "\n";
    cout << "\n";
    if(cartItems.size() == 0) cout << "Shopping cart is empty." << "\n";
    else for(Item item : cartItems) item.Print();
    cout << "\n";
    cout << "Total: $" << GetTotalCost() << "\n";

}

//prints all items in shopping cart, followed by the total cost
void ShoppingCart::PrintDescriptions() const {

    cout << name << "'s Shopping Cart - " << date << "\n";
    cout << "\n";
    cout << "Item Descriptions" << "\n";
    if(cartItems.size() == 0) cout << "Shopping cart is empty." << "\n";
    else for(Item item : cartItems) item.PrintDescription();

}