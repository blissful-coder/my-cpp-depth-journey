#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/**
 * BigNum class - A class for handling very large numbers
 * Supports operations like addition and multiplication for numbers with hundreds of digits
 */
class BigNum {
private:
    std::vector<int> digits; // Stores digits in reverse order for easier operations
    bool isNegative;        // Flag to indicate if the number is negative
    
    // Helper method to remove leading zeros
    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
        // If only a single zero remains, make sure it's not marked as negative
        if (digits.size() == 1 && digits[0] == 0) {
            isNegative = false;
        }
    }

public:
    // Default constructor - initializes to 0
    BigNum() : isNegative(false) {
        digits.push_back(0);
    }

    // Constructor from long long
    BigNum(long long num) {
        isNegative = (num < 0);
        if (num < 0) {
            num = -num; // Make positive for digit extraction
        }
        
        if (num == 0) {
            digits.push_back(0);
        } else {
            while (num > 0) {
                digits.push_back(num % 10);
                num /= 10;
            }
        }
    }

    // Constructor from string
    BigNum(const std::string& numStr) {
        if (numStr.empty()) {
            isNegative = false;
            digits.push_back(0);
            return;
        }
        
        size_t start = 0;
        isNegative = (numStr[0] == '-');
        if (isNegative || numStr[0] == '+') {
            start = 1;
        }
        
        // Initialize with 0 if the string is just a sign
        if (start >= numStr.size()) {
            digits.push_back(0);
            isNegative = false;
            return;
        }
        
        // Parse digits in reverse order
        for (int i = numStr.size() - 1; i >= static_cast<int>(start); --i) {
            if (!std::isdigit(numStr[i])) {
                throw std::invalid_argument("Invalid character in number string");
            }
            digits.push_back(numStr[i] - '0');
        }
        
        removeLeadingZeros();
    }

    // Addition operator
    BigNum operator+(const BigNum& other) const {
        // If signs are different, delegate to subtraction
        if (isNegative != other.isNegative) {
            if (isNegative) {
                // (-a) + b = b - a
                BigNum temp = *this;
                temp.isNegative = false;
                return other - temp;
            } else {
                // a + (-b) = a - b
                BigNum temp = other;
                temp.isNegative = false;
                return *this - temp;
            }
        }
        
        BigNum result;
        result.digits.clear();
        result.isNegative = isNegative; // Result has the same sign as both operands
        
        int carry = 0;
        size_t maxSize = std::max(digits.size(), other.digits.size());
        
        for (size_t i = 0; i < maxSize || carry; ++i) {
            int sum = carry;
            if (i < digits.size()) {
                sum += digits[i];
            }
            if (i < other.digits.size()) {
                sum += other.digits[i];
            }
            
            carry = sum / 10;
            result.digits.push_back(sum % 10);
        }
        
        result.removeLeadingZeros();
        return result;
    }
    
    // Subtraction helper - assumes |a| >= |b|
    static BigNum absoluteSubtract(const BigNum& a, const BigNum& b) {
        BigNum result;
        result.digits.clear();
        
        int borrow = 0;
        for (size_t i = 0; i < a.digits.size(); ++i) {
            int diff = a.digits[i] - borrow;
            if (i < b.digits.size()) {
                diff -= b.digits[i];
            }
            
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            
            result.digits.push_back(diff);
        }
        
        result.removeLeadingZeros();
        return result;
    }

    // Subtraction operator
    BigNum operator-(const BigNum& other) const {
        // If signs are different, delegate to addition
        if (isNegative != other.isNegative) {
            if (isNegative) {
                // (-a) - b = -(a + b)
                BigNum temp = *this;
                temp.isNegative = false;
                BigNum result = temp + other;
                result.isNegative = true;
                return result;
            } else {
                // a - (-b) = a + b
                BigNum temp = other;
                temp.isNegative = false;
                return *this + temp;
            }
        }
        
        // Compare absolute values to determine the result sign
        bool resultNegative = false;
        const BigNum* larger = this;
        const BigNum* smaller = &other;
        
        if (compareAbsoluteValue(other) < 0) {
            larger = &other;
            smaller = this;
            resultNegative = !isNegative; // Flip the sign
        } else {
            resultNegative = isNegative;
        }
        
        BigNum result = absoluteSubtract(*larger, *smaller);
        result.isNegative = resultNegative;
        result.removeLeadingZeros();
        return result;
    }

    // Multiplication operator
    BigNum operator*(const BigNum& other) const {
        // Handle special cases (multiplication by 0)
        if ((digits.size() == 1 && digits[0] == 0) || 
            (other.digits.size() == 1 && other.digits[0] == 0)) {
            return BigNum(0);
        }

        // Determine the sign of the result
        bool resultNegative = (isNegative != other.isNegative);
        
        // Initialize result with zeros
        BigNum result;
        result.digits.resize(digits.size() + other.digits.size(), 0);
        
        // Perform long multiplication algorithm
        for (size_t i = 0; i < digits.size(); ++i) {
            int carry = 0;
            for (size_t j = 0; j < other.digits.size() || carry; ++j) {
                long long product = result.digits[i + j] + carry;
                if (j < other.digits.size()) {
                    product += (long long)digits[i] * other.digits[j];
                }
                
                result.digits[i + j] = product % 10;
                carry = product / 10;
            }
        }
        
        result.isNegative = resultNegative;
        result.removeLeadingZeros();
        return result;
    }

    // Compare the absolute values of two BigNum objects
    int compareAbsoluteValue(const BigNum& other) const {
        if (digits.size() != other.digits.size()) {
            return (digits.size() > other.digits.size()) ? 1 : -1;
        }
        
        for (int i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i]) {
                return (digits[i] > other.digits[i]) ? 1 : -1;
            }
        }
        
        return 0; // Equal
    }
    
    // Compare operator
    bool operator==(const BigNum& other) const {
        return (isNegative == other.isNegative) && (digits == other.digits);
    }
    
    bool operator!=(const BigNum& other) const {
        return !(*this == other);
    }
    
    bool operator<(const BigNum& other) const {
        if (isNegative != other.isNegative) {
            return isNegative;
        }
        
        int absComp = compareAbsoluteValue(other);
        return isNegative ? absComp > 0 : absComp < 0;
    }
    
    bool operator<=(const BigNum& other) const {
        return (*this < other) || (*this == other);
    }
    
    bool operator>(const BigNum& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const BigNum& other) const {
        return !(*this < other);
    }
    
    // Convert to string representation
    std::string toString() const {
        if (digits.empty()) {
            return "0";
        }
        
        std::string result = isNegative ? "-" : "";
        for (int i = digits.size() - 1; i >= 0; --i) {
            result += std::to_string(digits[i]);
        }
        return result;
    }
    
    // Friend function to enable cout << BigNum
    friend std::ostream& operator<<(std::ostream& os, const BigNum& num) {
        os << num.toString();
        return os;
    }
};

// Test the BigNum implementation
int main() {
    // Test constructors
    BigNum a("12345678901234567890");
    BigNum b("98765432109876543210");
    BigNum c("-55555555555555555555");
    BigNum d = 0;
    BigNum e = 42;
    BigNum f = -24;
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "c = " << c << std::endl;
    std::cout << "d = " << d << std::endl;
    std::cout << "e = " << e << std::endl;
    std::cout << "f = " << f << std::endl;
    
    // Test addition
    std::cout << "\nAddition Tests:" << std::endl;
    std::cout << "a + b = " << (a + b) << std::endl;
    std::cout << "a + c = " << (a + c) << std::endl;
    std::cout << "e + f = " << (e + f) << std::endl;
    
    // Test subtraction
    std::cout << "\nSubtraction Tests:" << std::endl;
    std::cout << "b - a = " << (b - a) << std::endl;
    std::cout << "a - b = " << (a - b) << std::endl;
    std::cout << "a - c = " << (a - c) << std::endl;
    
    // Test multiplication
    std::cout << "\nMultiplication Tests:" << std::endl;
    std::cout << "a * d = " << (a * d) << std::endl;
    std::cout << "e * f = " << (e * f) << std::endl;
    std::cout << "a * 2 = " << (a * BigNum(2)) << std::endl;
    
    // Test very large numbers
    BigNum veryLarge1("9999999999999999999999999999999999999999999999999");
    BigNum veryLarge2("1");
    std::cout << "\nVery Large Number Tests:" << std::endl;
    std::cout << "veryLarge1 = " << veryLarge1 << std::endl;
    std::cout << "veryLarge1 + veryLarge2 = " << (veryLarge1 + veryLarge2) << std::endl;
    std::cout << "veryLarge1 * veryLarge2 = " << (veryLarge1 * veryLarge2) << std::endl;
    
    // Test with even larger number
    BigNum superLarge1("1234567890123456789012345678901234567890123456789012345678901234567890");
    BigNum superLarge2("9876543210987654321098765432109876543210987654321098765432109876543210");
    std::cout << "\nSuper Large Number Tests:" << std::endl;
    std::cout << "superLarge1 = " << superLarge1 << std::endl;
    std::cout << "superLarge2 = " << superLarge2 << std::endl;
    std::cout << "Sum = " << (superLarge1 + superLarge2) << std::endl;
    std::cout << "Product = " << (superLarge1 * superLarge2) << std::endl;
    
    return 0;
}
