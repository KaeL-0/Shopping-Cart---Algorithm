#include <iostream>
#include <fstream>
#include <iomanip>

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
    double price;
    int quantity;
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
/*
private:
	int countItems(Item* node);
    void collectItems(Item* node, Item** arr, int &index);
    void mergeSort(Item** arr, int left, int right);
    void merge(Item** arr, int left, int mid, int right);
    Item* insert(Item* node, string name, string category, double price, int quantity);
    Item* remove(Item* node, string name);
    void inorder(Item* node);
    void updateBSTFromFile();
    Item* findMax(Item* node);
    SoldItem* insertSoldItem(SoldItem* node, string name, int quantity);
}; */

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

void Supermarket::addItem(string name, string category, double price, int quantity) {
    root = insert(root, name, category, price, quantity);
}

Item* Supermarket::insert(Item* node, string name, string category, double price, int quantity) {
    if (!node) return new Item{name, category, price, quantity, nullptr, nullptr};
    if (name < node->name)
        node->left = insert(node->left, name, category, price, quantity);
    else
        node->right = insert(node->right, name, category, price, quantity);
    return node;
}

void Supermarket::removeItem(string name) {
    root = remove(root, name);
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

Item* Supermarket::findMax(Item* node) {
    while (node->right) node = node->right;
    return node;
}

void Supermarket::viewItems() {
	cout << "\n=== Shelf Items ===\n";
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

Supermarket::SoldItem* Supermarket::insertSoldItem(Supermarket::SoldItem* node, string name, int quantity) {
    if (!node) return new SoldItem{name, quantity, nullptr, nullptr};
    if (name < node->name)
        node->left = insertSoldItem(node->left, name, quantity);
    else if (name > node->name)
        node->right = insertSoldItem(node->right, name, quantity);
    else
        node->quantity += quantity; // Update quantity if item exists
    return node;
}


    Supermarket::SoldItem* findMaxSold(Supermarket::SoldItem* node) {
    if (!node) return nullptr;
    Supermarket::SoldItem* maxNode = node;
    Supermarket::SoldItem* leftMax = findMaxSold(node->left);
    Supermarket::SoldItem* rightMax = findMaxSold(node->right);
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
    while (file >> name >> quantity >> total) {
        soldRoot = insertSoldItem(soldRoot, name, quantity);
    }
    file.close();

    Supermarket::SoldItem* mostSold = findMaxSold(soldRoot);
    if (mostSold)
        cout << "Most Sold Item: " << mostSold->name << " - Quantity: " << mostSold->quantity << endl;
    else
        cout << "No items sold yet.\n";
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
    return -1; // Item not found
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

void Supermarket::collectItems(Item* node, Item** arr, int &index) {
    if (!node) return;
    collectItems(node->left, arr, index);
    arr[index++] = node;
    collectItems(node->right, arr, index);
}

void Supermarket::merge(Item** arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Item** L = new Item*[n1];
    Item** R = new Item*[n2];
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }
    int i = 0, j = 0, k = left;
    // Sort descending by quantity
    while (i < n1 && j < n2) {
        if (L[i]->quantity >= R[j]->quantity) {
            arr[k++] = L[i++];
        } else {
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
    Item** arr = new Item*[count];
    int index = 0;
    collectItems(root, arr, index);
    mergeSort(arr, 0, count - 1);
    
    cout << "\n=== Shelf Items Sorted by Quantity (Descending) ===\n";
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



void Customer::addToCart(string name, int quantity) {
    // Check if the item is already in the cart
    for (int i = 0; i < cartSize; i++) {
        if (cart[i].name == name) {
            if (supermarket->getStock(name) >= quantity) {
                cart[i].quantity += quantity;
                supermarket->updateStock(name, -quantity);
                cout << name << " quantity updated in cart.\n";
            } else {
                cout << "Not enough stock available.\n";
            }
            return;
        }
    }
    // If not found, add as a new cart entry
    if (supermarket->getStock(name) >= quantity) {
        double price = supermarket->getItemPrice(name);
        cart[cartSize++] = {name, price, quantity};
        supermarket->updateStock(name, -quantity);
        cout << name << " added to cart.\n";
    } else {
        cout << "Not enough stock available.\n";
    }
}




void Customer::removeFromCart(string name) {
    for (int i = 0; i < cartSize; i++) {
        if (cart[i].name == name) {
            supermarket->updateStock(name, cart[i].quantity);
            cart[i] = cart[--cartSize];
            return;
        }
    }
}

void Customer::viewCart() {
    if (cartSize == 0) {
        cout << "Cart is empty.\n";
        return;
    }
    
    quickSort(cart, 0, cartSize - 1);

	cout << "\n=== Cart Items ===\n";
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
        if (arr[j].quantity > pivot) { // Sorting in descending order
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
    
    cout << "Checkout Summary:\n";
    cout << "-----------------\n";
    for (int i = 0; i < cartSize; i++) {
        double itemTotal = cart[i].price * cart[i].quantity;
        totalAmount += itemTotal;
        cout << cart[i].name << " - Quantity: " << cart[i].quantity 
             << " - Total Price: " << itemTotal << endl;
        file << cart[i].name << " " << cart[i].quantity << " " << itemTotal << endl;
    }
    cout << "-----------------\n";
    cout << "Total Amount: " << totalAmount << endl;
    
    file.close();
    cartSize = 0;
}



int main() {
    Supermarket supermarket;
    Customer customer(&supermarket);
    int choice;
    string name, category;
    double price;
    int quantity;
    
supermarket.addItem("Apple", "Fruit", 1.50, 50);
supermarket.addItem("Banana", "Fruit", 0.50, 100);
supermarket.addItem("Orange", "Fruit", 0.75, 30);
supermarket.addItem("Milk", "Dairy", 2.00, 20);
supermarket.addItem("Bread", "Bakery", 1.00, 40);


    do {
    	 system("cls");
        cout << "1. Manager: Add Item\n2. Manager: Remove Item\n3. Manager: View Items\n4. Manager: Most Sold Item\n5. Manager: Sort Shelf by Quantity\n";
        cout << "6. Customer: Add to Cart\n7. Customer: Remove from Cart\n8. Customer: View Cart\n9. Customer: Checkout\n10. Exit \n";
        cout << "Enter choice: ";
        cin >> choice;
		if (cin.fail()) {
		    cin.clear();
		    cin.ignore(10000, '\n');
		    cout << "Invalid choice input. Please enter a number.\n";
		    system("pause");
		    continue; // Restart loop iteration
		}
        switch(choice) {
            case 1:
			    cout << "Enter name, category, price, quantity: ";
			    cin >> name >> category >> price >> quantity;
			    if (cin.fail()) {
			        cin.clear();
			        cin.ignore(10000, '\n');
			        cout << "Invalid input. Please try again.\n";
			        system("pause");
			        break;
			    }
			    supermarket.addItem(name, category, price, quantity);
			    system("pause");
			    break;

            case 2:
			    cout << "Enter item name to remove: ";
			    cin >> name;
			    if (cin.fail()) {
			        cin.clear();
			        cin.ignore(10000, '\n');
			        cout << "Invalid input. Please try again.\n";
			        system("pause");
			        break;
			    }
			    supermarket.removeItem(name);
			    system("pause");
			    break;

            case 3:
                supermarket.viewItems();
                system("pause");
                break;
            case 4:
			    supermarket.mostSoldItem();
			    system("pause");
			    break; 
			case 5:
			    supermarket.mergeSortShelf();
			    system("pause");
			    break;
			case 6:
			    cout << "Available Items:\n";
			    supermarket.viewItems();
			    cout << "Enter item name and quantity: ";
			    cin >> name >> quantity;
			    if (cin.fail()) {
			        cin.clear();
			        cin.ignore(10000, '\n');
			        cout << "Invalid input. Please try again.\n";
			        system("pause");
			        break;
			    }
			    customer.addToCart(name, quantity);
			    system("pause");
			    break;
            case 7:
			    cout << "Enter item name to remove from cart: ";
			    cin >> name;
			    if (cin.fail()) {
			        cin.clear();
			        cin.ignore(10000, '\n');
			        cout << "Invalid input. Please try again.\n";
			        system("pause");
			        break;
			    }
			    customer.removeFromCart(name);
			    system("pause");
			    break;

            case 8:
                customer.viewCart();
                system("pause");
                break;
            case 9:
                customer.checkout();
                system("pause");
                break;

        }
    } while (choice != 10);
    
return 0;
}

