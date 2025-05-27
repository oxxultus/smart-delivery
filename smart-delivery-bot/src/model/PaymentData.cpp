#include "PaymentData.h"
#include <ArduinoJson.h>

bool PaymentData::parseFromJson(const String& json) {
    JsonDocument doc;  // Updated to use JsonDocument (non-deprecated)
    DeserializationError error = deserializeJson(doc, json);
    if (error) return false;

    JsonObject obj = doc.as<JsonObject>();
    if (!obj["paymentId"].is<String>()) return false;  // Updated to recommended check
    paymentId = obj["paymentId"].as<String>();

    items.clear();
    for (JsonPair kv : obj) {
        String key = kv.key().c_str();
        if (key == "paymentId") continue;

        JsonArray arr = kv.value().as<JsonArray>();
        if (arr.size() != 2) continue;

        PaymentItem item;
        item.name = key;
        item.uid = arr[0].as<String>();
        item.quantity = arr[1].as<int>();
        items.push_back(item);
    }
    return true;
}

bool PaymentData::matchUID(const String& uid, String& name) {
    for (auto& item : items) {
        if (item.uid == uid) {
            name = item.name;
            return true;
        }
    }
    return false;
}

bool PaymentData::consumeItem(const String& uid) {
    for (auto& item : items) {
        if (item.uid == uid && item.quantity > 0) {
            item.quantity--;
            return true;
        }
    }
    return false;
}

void PaymentData::printItems() const {
    Serial.println("[결제 ID] " + paymentId);
    for (const auto& item : items) {
        Serial.println(" - " + item.name + ": UID=" + item.uid + ", 수량=" + String(item.quantity));
    }
}

String PaymentData::getPaymentId() const {
    return paymentId;
}

void PaymentData::clear() {
    items.clear();
    paymentId = "";
}