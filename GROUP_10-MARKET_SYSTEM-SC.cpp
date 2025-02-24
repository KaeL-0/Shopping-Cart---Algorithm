#include <iostream>
#include <fstream>
#include <iomanip>
#include <cctype> 
#include <string>
using namespace std;

struct Item {
    string name;
    string category;
    double price;
    int quantity;
    Item* left;
    Item* right;
};

struct CartItem {
    string name;
    double price = 0.0; 
    int quantity = 0;    
};

class Supermarket {
    Item* root;

public:
    struct SoldItem {
        string name;
        int quantity;
        SoldItem* left;
        SoldItem* right;
    };

    void mergeSortShelf();
    SoldItem* soldRoot = nullptr;
    Supermarket() : root(nullptr) {}
    void recordSale(string name, int quantity);
    void addItem(string name, string category, double price, int quantity);
    void removeItem(string name);
    void viewItems();
    void mostSoldItem();
    bool updateStock(string name, int quantityChange);
    int getStock(string name);
    double getItemPrice(string name);
    void loadItemsFromFile(const string& filename);
    void saveItemsToFile(const string& filename);
    void viewSalesHistory();
    void leastSoldItem();
    void displaySalesSummary();
    string toUpperCase(const string& str);

private:
    int countItems(Item* node);
    void collectItems(Item* node, Item** arr, int& index);
    void mergeSort(Item** arr, int left, int right);
    void merge(Item** arr, int left, int mid, int right);
    Item* insert(Item* node, string name, string category, double price, int quantity);
    Item* remove(Item* node, string name);
    void inorder(Item* node);
    void updateBSTFromFile();
    Item* findMax(Item* node);
    SoldItem* insertSoldItem(SoldItem* node, string name, int quantity);
    SoldItem* findMaxSold(SoldItem* node);
    void inorderSave(Item* node, ofstream& file);
    bool itemExists(string name);
};

class Customer {
    CartItem cart[100];
    int cartSize;
    Supermarket* supermarket;

public:
    Customer(Supermarket* sm) : cartSize(0), supermarket(sm) {}

    void addToCart(string name, int quantity);
    void removeFromCart(string name);
    void viewCart();
    void checkout();

private:
    void quickSort(CartItem arr[], int low, int high);
    int partition(CartItem arr[], int low, int high);
};

void clearScreen() {
#ifdef _WIN32
    system("cls"); 
#else
    system("clear"); 
#endif
}

string Supermarket::toUpperCase(const string& str) {
    string upperStr;
    for (char c : str) {
        upperStr += toupper(c);
    }
    return upperStr;
}

void Supermarket::addItem(string name, string category, double price, int quantity) {
    name = toUpperCase(name);
    category = toUpperCase(category);

    Item* current = root;
    while (current) {
        if (current->name == name) {
            current->quantity += quantity;
            cout << "Quantity updated for '" << name << "'\n";
            saveItemsToFile("items.txt");
            return;
        }
        current = (name < current->name) ? current->left : current->right;
    }


    if (price < 0 || quantity < 0) {
        cout << "Price and quantity must be non-negative.\n";
        return;
    }

    root = insert(root, name, category, price, quantity);
    cout << "Item addded successfully!\n";
    saveItemsToFile("items.txt");
}

bool Supermarket::itemExists(string name) {
    string upperName = toUpperCase(name);
    Item* current = root;
    while (current) {
        if (name == current->name) return true;
        if (name < current->name)
            current = current->left;
        else
            current = current->right;
    }
    return false;
}

Item* Supermarket::insert(Item* node, string name, string category, double price, int quantity) {
    if (!node) return new Item{ name, category, price, quantity, nullptr, nullptr };
    if (name < node->name)
        node->left = insert(node->left, name, category, price, quantity);
    else
        node->right = insert(node->right, name, category, price, quantity);
    return node;
}

void Supermarket::viewItems() {
    cout << "\n=================================================\n";
    cout << "                   View Catalog\n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(15) << "Name"
        << setw(15) << "Category"
        << setw(10) << "Price"
        << setw(10) << "Quantity" << endl;
    inorder(root);
}

void Supermarket::inorder(Item* node) {
    if (!node) return;
    inorder(node->left);
    cout << left << setw(15) << node->name
        << setw(15) << node->category
        << setw(10) << node->price
        << setw(10) << node->quantity << endl;
    inorder(node->right);
}

int Supermarket::getStock(string name) {
    Item* current = root;
    while (current) {
        if (name == current->name) return current->quantity;
        if (name < current->name)
            current = current->left;
        else
            current = current->right;
    }
    return 0;
}

Supermarket::SoldItem* Supermarket::insertSoldItem(SoldItem* node, string name, int quantity) {
    if (!node) return new SoldItem{ name, quantity, nullptr, nullptr };
    if (name < node->name)
        node->left = insertSoldItem(node->left, name, quantity);
    else if (name > node->name)
        node->right = insertSoldItem(node->right, name, quantity);
    else
        node->quantity += quantity; 
    return node;
}

Supermarket::SoldItem* Supermarket::findMaxSold(SoldItem* node) {
    if (!node) return nullptr;
    SoldItem* maxNode = node;
    SoldItem* leftMax = findMaxSold(node->left);
    SoldItem* rightMax = findMaxSold(node->right);
    if (leftMax && leftMax->quantity > maxNode->quantity)
        maxNode = leftMax;
    if (rightMax && rightMax->quantity > maxNode->quantity)
        maxNode = rightMax;
    return maxNode;
}

void Supermarket::mostSoldItem() {
    ifstream file("checkout.txt");
    if (!file) {
        cout << "No sales data available.\n";
        return;
    }

    string name;
    int quantity;
    double total;

    const int MAX_ITEMS = 100; 
    string mostSoldItems[MAX_ITEMS]; 
    int highestQuantity = 0;
    int itemCount = 0;

    while (file >> name >> quantity >> total) {
        if (quantity > highestQuantity) {
            highestQuantity = quantity;
            itemCount = 0; 
            mostSoldItems[itemCount++] = name; 
        }
        else if (quantity == highestQuantity) {
            if (itemCount < MAX_ITEMS) {
                mostSoldItems[itemCount++] = name; 
            }
        }
    }

    file.close();

    if (highestQuantity == 0) {
        cout << "No items sold yet.\n";
    }
    else {
        cout << "Most Sold Item(s): ";
        for (int i = 0; i < itemCount; i++) {
            cout << "\n" << mostSoldItems[i];
        }
        cout << "\n\nQuantity: " << highestQuantity << endl;
    }
}

double Supermarket::getItemPrice(string name) {
    Item* current = root;
    while (current) {
        if (current->name == name)
            return current->price;
        else if (name < current->name)
            current = current->left;
        else
            current = current->right;
    }
    return -1; 
}

void Supermarket::recordSale(string name, int quantity) {
    soldRoot = insertSoldItem(soldRoot, name, quantity);
}

bool Supermarket::updateStock(string name, int quantityChange) {
    Item* current = root;
    while (current) {
        if (name == current->name) {
            current->quantity += quantityChange;
            return true;
        }
        if (name < current->name)
            current = current->left;
        else
            current = current->right;
    }
    return false;
}

int Supermarket::countItems(Item* node) {
    if (!node) return 0;
    return 1 + countItems(node->left) + countItems(node->right);
}

void Supermarket::collectItems(Item* node, Item** arr, int& index) {
    if (!node) return;
    collectItems(node->left, arr, index);
    arr[index++] = node;
    collectItems(node->right, arr, index);
}

void Supermarket::merge(Item** arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Item** L = new Item * [n1];
    Item** R = new Item * [n2];
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i]->quantity >= R[j]->quantity) {
            arr[k++] = L[i++];
        }
        else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) {
        arr[k++] = L[i++];
    }
    while (j < n2) {
        arr[k++] = R[j++];
    }
    delete[] L;
    delete[] R;
}

void Supermarket::mergeSort(Item** arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void Supermarket::mergeSortShelf() {
    int count = countItems(root);
    if (count == 0) {
        cout << "No items in shelf.\n";
        return;
    }
    Item** arr = new Item * [count];
    int index = 0;
    collectItems(root, arr, index);
    mergeSort(arr, 0, count - 1);

    cout << "\n=================================================\n";
    cout << "                  Merge Sort Shelf\n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(15) << "Name"
        << setw(15) << "Category"
        << setw(10) << "Price"
        << setw(10) << "Quantity" << endl;

    for (int i = 0; i < count; i++) {
        cout << left << setw(15) << arr[i]->name
            << setw(15) << arr[i]->category
            << setw(10) << arr[i]->price
            << setw(10) << arr[i]->quantity << endl;
    }
    delete[] arr;
}

void Supermarket::removeItem(string name) {
    string upperName = toUpperCase(name);

    if (!itemExists(upperName)) {
        cout << "Item with name '" << name << "' does not exist. Cannot remove.\n";
        return; 
    }

    root = remove(root, upperName); 
    cout << "Item removed successfully!\n";
    saveItemsToFile("items.txt"); 
}

Item* Supermarket::findMax(Item* node) {
    while (node && node->right) {
        node = node->right;
    }
    return node;
}

Item* Supermarket::remove(Item* node, string name) {
    if (!node) return nullptr;
    if (name < node->name)
        node->left = remove(node->left, name);
    else if (name > node->name)
        node->right = remove(node->right, name);
    else {
        if (!node->left) return node->right;
        if (!node->right) return node->left;
        Item* maxLeft = findMax(node->left);
        node->name = maxLeft->name;
        node->category = maxLeft->category;
        node->price = maxLeft->price;
        node->quantity = maxLeft->quantity;
        node->left = remove(node->left, maxLeft->name);
    }
    return node;
}

void Customer::addToCart(string name, int quantity) {
    string upperName = supermarket->toUpperCase(name);

    for (int i = 0; i < cartSize; i++) {
        if (cart[i].name == upperName) {
            if (supermarket->getStock(upperName) >= quantity) {
                cart[i].quantity += quantity; 
                cout << name << " quantity updated in cart.\n";
            }
            else {
                cout << "Not enough stock available.\n";
            }
            return;
        }
    }
    if (supermarket->getStock(upperName) >= quantity) {
        double price = supermarket->getItemPrice(upperName);
        cart[cartSize++] = { upperName, price, quantity }; 
        cout << name << " added to cart.\n";
        supermarket->updateStock(upperName, -quantity);
    }
    else {
        cout << "Not enough stock available.\n";
    }
}

void Customer::removeFromCart(string name) {
    string upperName = supermarket->toUpperCase(name);

    for (int i = 0; i < cartSize; i++) {
        if (cart[i].name == upperName) {
            supermarket->updateStock(upperName, cart[i].quantity);
            cart[i] = cart[--cartSize]; 
            cout << name << " removed from cart.\n";
            return;
        }
    }
    cout << "Item not found in cart.\n";
    

}

void Customer::viewCart() {
    if (cartSize == 0) {
        cout << "Cart is empty.\n";
        return;
    }

    quickSort(cart, 0, cartSize - 1);

    cout << "=================================================\n";
    cout << "                   Cart Items\n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(15) << "Name"
        << setw(10) << "Price"
        << setw(10) << "Quantity"
        << setw(15) << "Total Price" << endl;

    for (int i = 0; i < cartSize; i++) {
        cout << left << setw(15) << cart[i].name
            << setw(10) << cart[i].price
            << setw(10) << cart[i].quantity
            << setw(15) << (cart[i].price * cart[i].quantity)
            << endl;
    }
}

void Customer::quickSort(CartItem arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int Customer::partition(CartItem arr[], int low, int high) {
    int pivot = arr[high].quantity;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j].quantity > pivot) { 
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void Customer::checkout() {
    if (cartSize == 0) {
        cout << "Cart is empty. Nothing to checkout.\n";
        return;
    }

    ofstream file("checkout.txt", ios::app);
    double totalAmount = 0;

    cout << "=================================================\n";
    cout << "             Checkout Summary:\n";
    cout << "-------------------------------------------------\n";
    for (int i = 0; i < cartSize; i++) {
        double itemTotal = cart[i].price * cart[i].quantity;
        totalAmount += itemTotal;
        cout << cart[i].name << " - Quantity: " << cart[i].quantity
            << " - Total Price: " << itemTotal << endl;
        file << cart[i].name << " " << cart[i].quantity << " " << itemTotal << endl;

        supermarket->updateStock(cart[i].name, -cart[i].quantity);
    }
    cout << "-------------------------------------------------\n";
    cout << "Total Amount: " << totalAmount << endl;

    file.close();
    cartSize = 0; 

    supermarket->saveItemsToFile("items.txt");
}

void Supermarket::loadItemsFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string name, category;
        double price;
        int quantity;

        while (file >> name >> category >> price >> quantity) {
            addItem(name, category, price, quantity); 
        }
        file.close();
    }
    else {
        cout << "Error opening file: " << filename << endl;
    }
}

void Supermarket::saveItemsToFile(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        inorderSave(root, file);
        file.close();
    }
    else {
        cout << "Error opening file: " << filename << endl;
    }
}

void Supermarket::inorderSave(Item* node, ofstream& file) {
    if (!node) return;
    inorderSave(node->left, file);
    file << node->name << " " << node->category << " " << node->price << " " << node->quantity << endl;
    inorderSave(node->right, file);
}

void Supermarket::viewSalesHistory() {
    ifstream file("checkout.txt");
    if (!file) {
        cout << "No sales history available.\n";
        return;
    }

    string name;
    int quantity;
    double total;

    cout << "=================================================\n";
    cout << "                  Sales History\n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(20) << "Item Name"
        << setw(10) << "Quantity"
        << setw(10) << "Total Price" << endl;
    cout << "-------------------------------------------------\n";

    while (file >> name >> quantity >> total) {
        cout << left << setw(20) << name
            << setw(10) << quantity
            << setw(10) << total << endl;
    }

    file.close();
}

void Supermarket::leastSoldItem() {
    ifstream file("checkout.txt");
    if (!file) {
        cout << "No sales data available.\n";
        return;
    }

    string name;
    int quantity;
    double total;

    const int MAX_ITEMS = 100; 
    string leastSoldItems[MAX_ITEMS]; 
    int leastQuantity = INT_MAX;
    int itemCount = 0; 

    while (file >> name >> quantity >> total) {
        if (quantity < leastQuantity) {
            leastQuantity = quantity;
            itemCount = 0; 
            leastSoldItems[itemCount++] = name; 
        }
        else if (quantity == leastQuantity) {
            if (itemCount < MAX_ITEMS) {
                leastSoldItems[itemCount++] = name; 
            }
        }
    }

    file.close();

    if (leastQuantity == INT_MAX) {
        cout << "No items sold.\n";
    }
    else {
        cout << "Least Sold Item(s): ";
        for (int i = 0; i < itemCount; i++) {
            cout << "\n" << leastSoldItems[i];
        }
        cout << "\n\nQuantity: " << leastQuantity << endl;
    }
}

void Supermarket::displaySalesSummary() {
    ifstream file("checkout.txt");
    if (!file) {
        cout << "No sales data available.\n";
        return;
    }

    string name;
    int quantity;
    double total;
    double totalSales = 0.0;
    int transactionCount = 0;

    const int MAX_ITEMS = 100; 
    string itemNames[MAX_ITEMS];
    int itemQuantities[MAX_ITEMS] = { 0 };
    double itemTotals[MAX_ITEMS] = { 0.0 };
    int itemCount = 0;

    while (file >> name >> quantity >> total) {
        totalSales += total; 
        transactionCount++; 

        bool found = false;
        for (int i = 0; i < itemCount; i++) {
            if (itemNames[i] == name) {
                itemQuantities[i] += quantity; 
                itemTotals[i] += total;
                found = true;
                break;
            }
        }

        if (!found && itemCount < MAX_ITEMS) {
            itemNames[itemCount] = name;
            itemQuantities[itemCount] = quantity;
            itemTotals[itemCount] = total;
            itemCount++;
        }
    }

    file.close();


    cout << "=================================================\n";
    cout << "      Sales Summary      \n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(20) << "Item" << setw(10) << "Quantity" << setw(10) << "Total Price" << endl;
    cout << "-------------------------------------------------\n";
    for (int i = 0; i < itemCount; i++) {
        cout << left << setw(20) << itemNames[i] << setw(10) << itemQuantities[i] << "$" << itemTotals[i] << endl;
    cout << "-------------------------------------------------\n";

    }
    cout << "Total Sales: $" << totalSales << endl;
    cout << "Number of Transactions: " << transactionCount << endl;

    cout << "=================================================\n";
}

int main() {
    Supermarket supermarket;
    Customer customer(&supermarket);
    supermarket.loadItemsFromFile("items.txt");
    int choice, role, opt1, opt2;
    string name, category;
    double price;
    int quantity;

    do {
        clearScreen();
        cout << "=================================================\n";
        cout << "              IN-GAME MARKET SYSTEM\n";
        cout << "-------------------------------------------------\n";
        cout << "                   Select Role\n\n";
        cout << "                  [1] Manager\n";
        cout << "                  [2] Customer\n";
        cout << "                  [0] Exit\n";
        cout << "-------------------------------------------------\n";
        cout << "Enter choice: ";
        cin >> role;

        while (cin.fail() || (role < 0 || role > 2)) {
            cin.clear(); 
            cin.ignore(10000, '\n'); 
            cout << "Invalid choice. Please enter 0, 1, or 2: ";
            cin >> role;
        }

        if (role == 1) {
            do {
                clearScreen();
                cout << "=================================================\n";
                cout << "                  Manager Menu \n";
                cout << "-------------------------------------------------\n";
                cout << "            [1] Add Item\n";
                cout << "            [2] Remove Item\n";
                cout << "            [3] View Catalog\n";
                cout << "            [4] Sort Catalog by Quantity\n";
                cout << "            [5] Sales History\n";
                cout << "            [6] Sales Summary\n";
                cout << "            [0] Back\n";
                cout << "-------------------------------------------------\n";
                cout << "Enter choice: ";
                cin >> choice;

                while (cin.fail() || (choice < 0 || choice > 6)) {
                    cin.clear(); 
                    cin.ignore(10000, '\n'); 
                    cout << "Invalid choice. Please enter 0 to 6: ";
                    cin >> choice;
                }

                switch (choice) {
                case 1:
                    cout << "\n=================================================\n";
                    cout << "                    Add Item \n";
                    cout << "-------------------------------------------------\n";
                    cout << "Enter name: ";
                    cin >> name;
                    cout << "Enter category: "; 
                    cin >> category;
                    cout << "Enter price: ";
                    cin >> price;
                    cout << "Enter quantity: ";
                    cin >> quantity;

                    while (cin.fail() || price < 0 || quantity < 0) {
                        cin.clear(); 
                        cin.ignore(10000, '\n'); 
                        cout << "Invalid input. Price and quantity must be non-negative. Please enter again: ";
                        cin >> price >> quantity;
                    }

                    supermarket.addItem(name, category, price, quantity);
                    cout << "-------------------------------------------------\n";
                    system("pause");
                    break;
                case 2:
                    cout << "\n=================================================\n";
                    cout << "                   Remove item \n";
                    cout << "-------------------------------------------------\n";
                    cout << "Enter item name: ";
                    cin >> name;
                    supermarket.removeItem(name);
                    cout << "-------------------------------------------------\n";
                    system("pause");
                    break;
                case 3:
                    supermarket.viewItems();
                    cout << "-------------------------------------------------\n";
                    system("pause");
                    break;
                case 4:
                    supermarket.mergeSortShelf();
                    cout << "-------------------------------------------------\n";
                    system("pause");
                    break;
                case 5:
                    clearScreen();
                    do {
                        supermarket.viewSalesHistory();
                        cout << "-------------------------------------------------\n";
                        cout << "[1] Most Sold Item\n";
                        cout << "[2] Least Sold Item\n";
                        cout << "[0] Back\n";
                        cout << "-------------------------------------------------\n";
                        cout << "Enter choice: ";
                        cin >> opt2;

                        while (cin.fail() || (opt2 < 0 || opt2 > 2)) {
                            cin.clear();
                            cin.ignore(10000, '\n');
                            cout << "Invalid choice. Please enter 0 to 2: ";
                            cin >> opt2;
                        }

                        switch (opt2) {
                            case 1: {
                                cout << "\n=================================================\n";
                                cout << "                 Most Sold Item(s) \n";
                                cout << "-------------------------------------------------\n";
                                supermarket.mostSoldItem();
                                cout << "-------------------------------------------------\n";
                                system("pause");
                                clearScreen();
                                break;
                            }
                            case 2: {
                                cout << "\n=================================================\n";
                                cout << "                Least Sold Item(s) \n";
                                cout << "-------------------------------------------------\n";
                                supermarket.leastSoldItem();

                                cout << "-------------------------------------------------\n";
                                system("pause");
                                clearScreen();
                                break;
                            }
                        }
                    } while (opt2 != 0);
                    break;
                case 6:
                    clearScreen();
                    supermarket.displaySalesSummary();
                    system("pause");
                    break;
                }
            } while (choice != 0);
        }
        else if (role == 2) {
            do {
                clearScreen();
                cout << "=================================================\n";
                cout << "                  Customer Menu\n";
                cout << "-------------------------------------------------\n";
                cout << "                [1] Add to Cart\n";
                cout << "                [2] View Cart\n";
                cout << "                [3] Checkout\n";
                cout << "                [0] Back\n";
                cout << "-------------------------------------------------\n";
                cout << "Enter choice: ";
                cin >> choice;

                while (cin.fail() || (choice < 0 || choice > 3)) {
                    cin.clear(); 
                    cin.ignore(10000, '\n'); 
                    cout << "Invalid choice. Please enter 0 to 3: ";
                    cin >> choice;
                }

                switch (choice) {
                case 1:
                    supermarket.viewItems();
                    cout << "-------------------------------------------------\n";
                    cout << "Enter item name: ";
                    cin >> name;
                    cout << "Enter item quantity: ";
                    cin >> quantity;

                    while (cin.fail() || quantity < 1) {
                        cin.clear(); 
                        cin.ignore(10000, '\n'); 
                        cout << "Invalid quantity. Please enter a positive number: ";
                        cin >> quantity;
                    }

                    customer.addToCart(name, quantity);
                    cout << "-------------------------------------------------\n";
                    system("pause");
                    break;
                case 2:
                    clearScreen();
                    do {
                        customer.viewCart();
                        cout << "-------------------------------------------------\n";
                        cout << "[1] Remove Items from Cart\n";
                        cout << "[2] Checkout\n";
                        cout << "[0] Back\n";
                        cout << "-------------------------------------------------\n";
                        cout << "Enter choice: ";
                        cin >> opt1;

                        while (cin.fail() || (opt1 < 0 || opt1 > 2)) {
                            cin.clear(); 
                            cin.ignore(10000, '\n'); 
                            cout << "Invalid choice. Please enter 0 to 2: ";
                            cin >> opt1;
                        }

                        switch (opt1) {
                        case 1: {
                            cout << "Enter item name to remove from cart: ";
                            cin >> name;
                            customer.removeFromCart(name);
                            cout << "-------------------------------------------------\n";
                            system("pause");
                            clearScreen();
                            break;
                        }
                        case 2: {
                            clearScreen();
                            customer.checkout();
                            cout << "-------------------------------------------------\n";
                            system("pause");
                            opt1 = 0;
                            break;
                        }
                        }
                    } while (opt1 != 0);
                    break;
                case 3:
                    clearScreen();
                    customer.checkout();
                    cout << "-------------------------------------------------\n";
                    system("pause");
                    break;
                }
            } while (choice != 0);
        }
    } while (role != 0);

    return 0;
}
