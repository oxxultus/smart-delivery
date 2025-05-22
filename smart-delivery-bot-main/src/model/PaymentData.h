#ifndef PAYMENTDATA_H
#define PAYMENTDATA_H

#include <Arduino.h>
#include <vector>

struct PaymentItem {
    String name;
    String uid;
    int quantity;
};

class PaymentData {
private:
    String paymentId;
    std::vector<PaymentItem> items;

public:
    bool parseFromJson(const String& json);
    bool matchUID(const String& uid, String& name);
    bool consumeItem(const String& uid);
    void printItems() const;
    String getPaymentId() const;

    void clear();
};

#endif // PAYMENTDATA_H