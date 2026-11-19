#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

template <typename T>
struct Node {
    T value;
    Node* prev;
    Node* next;

    Node(const T& v) : value(v), prev(nullptr), next(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    Node<T>* head_;
    Node<T>* tail_;
    std::size_t size_;

    void clear() {
        Node<T>* cur = head_;
        while (cur != nullptr) {
            Node<T>* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    void copyFrom(const LinkedList& other) {
        Node<T>* cur = other.head_;
        while (cur != nullptr) {
            pushTail(cur->value);
            cur = cur->next;
        }
    }
public:
    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    LinkedList(const LinkedList& other)
        : head_(nullptr), tail_(nullptr), size_(0) {
        copyFrom(other);
    }

    ~LinkedList() {
        clear();
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        copyFrom(other);
        return *this;
    }

    void pushTail(const T& value) {
        Node<T>* node = new Node<T>(value);
        node->prev = tail_;
        if (tail_ != nullptr) {
            tail_->next = node;
        }
        else {
            head_ = node;
        }
        tail_ = node;
        ++size_;
    }

    void pushTail(const LinkedList& other) {
        if (this == &other) {
            LinkedList copy(other);
            pushTail(copy);
            return;
        }
        Node<T>* cur = other.head_;
        while (cur != nullptr) {
            pushTail(cur->value);
            cur = cur->next;
        }
    }

    void pushHead(const T& value) {
        Node<T>* node = new Node<T>(value);
        node->next = head_;
        if (head_ != nullptr) {
            head_->prev = node;
        }
        else {
            tail_ = node;
        }
        head_ = node;
        ++size_;
    }

    void pushHead(const LinkedList& other) {
        if (this == &other) {
            LinkedList copy(other);
            pushHead(copy);
            return;
        }

        Node<T>* cur = other.tail_;
        while (cur != nullptr) {
            pushHead(cur->value);
            cur = cur->prev;
        }
    }

    void popHead() {
        if (head_ == nullptr) {
            throw std::out_of_range("LinkedList::popHead: list is empty");
        }
        Node<T>* old = head_;
        head_ = head_->next;
        if (head_ != nullptr) {
            head_->prev = nullptr;
        }
        else {
            tail_ = nullptr;
        }
        delete old;
        --size_;
    }

    void popTail() {
        if (tail_ == nullptr) {
            throw std::out_of_range("LinkedList::popTail: list is empty");
        }
        Node<T>* old = tail_;
        tail_ = tail_->prev;
        if (tail_ != nullptr) {
            tail_->next = nullptr;
        }
        else {
            head_ = nullptr;
        }
        delete old;
        --size_;
    }

    std::size_t deleteNode(const T& value) {
        std::size_t removed = 0;
        Node<T>* cur = head_;
        while (cur != nullptr) {
            Node<T>* nextNode = cur->next;
            if (cur->value == value) {
                if (cur->prev != nullptr) {
                    cur->prev->next = cur->next;
                }
                else {
                    head_ = cur->next;
                }
                if (cur->next != nullptr) {
                    cur->next->prev = cur->prev;
                }
                else {
                    tail_ = cur->prev;
                }
                delete cur;
                ++removed;
                --size_;
            }
            cur = nextNode;
        }
        return removed;
    }

    T& at(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("LinkedList::at: index out of range");
        }
        Node<T>* cur = head_;
        for (std::size_t i = 0; i < index; ++i) {
            cur = cur->next;
        }
        return cur->value;
    }

    const T& at(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("LinkedList::at: index out of range");
        }
        Node<T>* cur = head_;
        for (std::size_t i = 0; i < index; ++i) {
            cur = cur->next;
        }
        return cur->value;
    }

    std::size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void print(std::ostream& os) const {
        os << "[";
        Node<T>* cur = head_;
        while (cur != nullptr) {
            os << cur->value;
            if (cur->next != nullptr) {
                os << ", ";
            }
            cur = cur->next;
        }
        os << "]";
    }

};

template <typename T>
std::ostream& operator<<(std::ostream& os, const LinkedList<T>& list) {
    list.print(os);
    return os;
}

using Apartment = LinkedList<std::string>;

std::string randomName(std::mt19937& rng);

class AddressBook {
private:
    Apartment* apartments_;
    std::size_t count_;

    void clear() {
        delete[] apartments_;
        apartments_ = nullptr;
        count_ = 0;
    }
public:
    AddressBook() : apartments_(nullptr), count_(0) {}

    AddressBook(std::size_t apartments,
        std::size_t residentCount, std::mt19937& rng)
        : apartments_(nullptr), count_(0) {

        if (apartments == 0) {
            throw std::invalid_argument(
                "AddressBook: apartment count must be positive");
        }

        apartments_ = new Apartment[apartments];
        count_ = apartments;

        std::uniform_int_distribution<std::size_t> distribution(
            0, apartments - 1
        );

        for (std::size_t i = 0; i < residentCount; ++i) {
            std::size_t index = distribution(rng);
            apartments_[index].pushTail(randomName(rng));
        }
    }

    AddressBook(const AddressBook& other)
        : apartments_(nullptr), count_(0) {
        if (other.count_ > 0) {
            apartments_ = new Apartment[other.count_];
            count_ = other.count_;
            for (std::size_t i = 0; i < count_; ++i) {
                apartments_[i] = other.apartments_[i];
            }
        }
    }

    ~AddressBook() {
        clear();
    }

    AddressBook& operator=(const AddressBook& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        if (other.count_ > 0) {
            apartments_ = new Apartment[other.count_];
            count_ = other.count_;
            for (std::size_t i = 0; i < count_; ++i) {
                apartments_[i] = other.apartments_[i];
            }
        }
        return *this;
    }

    std::size_t apartmentCount() const {
        return count_;
    }

    Apartment& apartment(std::size_t index) {
        if (index >= count_) {
            throw std::out_of_range("AddressBook::apartment: index out of range");
        }
        return apartments_[index];
    }

    const Apartment& apartment(std::size_t index) const {
        if (index >= count_) {
            throw std::out_of_range("AddressBook::apartment: index out of range");
        }
        return apartments_[index];
    }

    void print(std::ostream& os) const {
        os << "Address book (" << count_ << " apartments):\n";
        for (std::size_t i = 0; i < count_; ++i) {
            os << "  Apt " << (i + 1) << ": " << apartments_[i] << "\n";
        }
    }

};

std::string randomName(std::mt19937& rng) {
    const char* names[] = {
        "Ivan",  "Petr",   "Anna",    "Maria",   "Olga",    "Sergey",
        "Dmitry", "Elena", "Alexey",  "Tatiana", "Nikolay", "Svetlana" };
    const std::size_t n = sizeof(names) / sizeof(names[0]);
    std::uniform_int_distribution<std::size_t> distribution(0, n - 1);
    return names[distribution(rng)];
}

std::ostream& operator<<(std::ostream& os, const AddressBook& book) {
    book.print(os);
    return os;
}

int main() {
    try {
        std::random_device rd;
        std::mt19937 rng(rd());

        std::cout << "Demo LinkedList<int> (doubly):\n";

        LinkedList<int> ints;
        for (int i = 1; i <= 5; ++i) {
            ints.pushTail(i);
        }

        std::cout << "  " << ints << "\n";

        LinkedList<int> copy(ints);
        copy.pushHead(0);

        std::cout << "  copy after pushHead(0): "
            << copy << "\n";

        copy.popHead();
        copy.popTail();

        std::cout << "  copy after popHead/popTail: "
            << copy << "\n";

        std::size_t removed = copy.deleteNode(3);

        std::cout << "  removed " << removed
            << " nodes with value 3: " << copy << "\n";

        std::cout << "\nDemo AddressBook:\n";

        std::size_t apartments = 5;
        std::size_t residentCount = 10;

        AddressBook book(apartments, residentCount, rng);
        std::cout << book;

        std::cout << "\nApartment 2 residents: "
            << book.apartment(1) << "\n";

    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
