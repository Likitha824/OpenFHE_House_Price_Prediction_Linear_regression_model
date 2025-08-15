#include "openfhe.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include <iostream>
#include <fstream>

using namespace lbcrypto;

int main() {
    std::cout << "Initializing FHE parameters with reduced memory requirements..." << std::endl;
    
    // Setup parameters with reduced memory requirements
    // These changes will generate much smaller keys while still supporting
    // the basic operations needed for housing price prediction
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(10);    //  consecutive multiplications
    parameters.SetScalingModSize(50);        //  precision but uses less memory
    parameters.SetBatchSize(8192);
    parameters.SetRingDim(32768);
    parameters.SetFirstModSize(60);          //  processes fewer values simultaneously but reduces memory needs
    
    // Generate crypto context
    auto cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    
    std::cout << "Generating keys with smaller parameters..." << std::endl;
    
    // Key generation
    auto keyPair = cc->KeyGen();
    if (!keyPair.good()) {
        std::cerr << "Key generation failed!" << std::endl;
        return 1;
    }
    
    // Generate multiplication evaluation keys (smaller with new parameters)
    cc->EvalMultKeyGen(keyPair.secretKey);
    
    // Generate rotation keys (needed for certain operations)
    std::vector<int32_t> rotations = {1, 2, 4, 8, 16, 32};
    cc->EvalRotateKeyGen(keyPair.secretKey, rotations);
    
    std::cout << "Serializing keys and context..." << std::endl;
    
    // Save a sample data file for testing
    std::ofstream testFile("test.txt");
    if (!testFile.is_open()) {
        std::cerr << "Cannot create files in this directory!" << std::endl;
        return 1;
    }
    // Write the 13 features for housing price prediction (example values)
    testFile << "-122.23" << std::endl;  // longitude
    testFile << "37.88" << std::endl;    // latitude
    testFile << "41.0" << std::endl;     // housing_median_age
    testFile << "880.0" << std::endl;    // total_rooms
    testFile << "129.0" << std::endl;    // total_bedrooms
    testFile << "322.0" << std::endl;    // population
    testFile << "126.0" << std::endl;    // households
    testFile << "8.3252" << std::endl;   // median_income
    testFile << "0" << std::endl;        // ocean_proximity category 1
    testFile << "0" << std::endl;        // ocean_proximity category 2
    testFile << "0" << std::endl;        // ocean_proximity category 3
    testFile << "1" << std::endl;        // ocean_proximity category 4
    testFile << "0" << std::endl;        // ocean_proximity category 5
    testFile.close();
    
    try {
        // Serialize crypto context
        std::ofstream ccFile("cryptocontext.bin", std::ios::binary);
        if (!ccFile.is_open()) {
            std::cerr << "Cannot open cryptocontext.bin for writing" << std::endl;
            return 1;
        }
        Serial::Serialize(cc, ccFile, SerType::BINARY);
        ccFile.close();
        std::cout << "Crypto context serialized successfully." << std::endl;
        
        // Serialize public key
        std::ofstream pubKeyFile("key-public.bin", std::ios::binary);
        if (!pubKeyFile.is_open()) {
            std::cerr << "Cannot open key-public.bin for writing" << std::endl;
            return 1;
        }
        Serial::Serialize(keyPair.publicKey, pubKeyFile, SerType::BINARY);
        pubKeyFile.close();
        std::cout << "Public key serialized successfully." << std::endl;
        
        // Serialize multiplication key directly from crypto context
        std::ofstream multKeyFile("key-mult.bin", std::ios::binary);
        if (!multKeyFile.is_open()) {
            std::cerr << "Cannot open key-mult.bin for writing" << std::endl;
            return 1;
        }
        if (!cc->SerializeEvalMultKey(multKeyFile, SerType::BINARY)) {
            std::cerr << "Failed to serialize multiplication key" << std::endl;
            return 1;
        }
        multKeyFile.close();
        std::cout << "Multiplication key serialized successfully." << std::endl;
        
        // Serialize rotation key directly from crypto context
        std::ofstream rotKeyFile("key-rot.bin", std::ios::binary);
        if (!rotKeyFile.is_open()) {
            std::cerr << "Cannot open key-rot.bin for writing" << std::endl;
            return 1;
        }
        if (!cc->SerializeEvalAutomorphismKey(rotKeyFile, SerType::BINARY)) {
            std::cerr << "Failed to serialize rotation key" << std::endl;
            return 1;
        }
        rotKeyFile.close();
        std::cout << "Rotation key serialized successfully." << std::endl;
        
        // Serialize secret key (optional - only needed for decryption)
        std::ofstream secKeyFile("key-secret.bin", std::ios::binary);
        if (!secKeyFile.is_open()) {
            std::cerr << "Cannot open key-secret.bin for writing" << std::endl;
            return 1;
        }
        Serial::Serialize(keyPair.secretKey, secKeyFile, SerType::BINARY);
        secKeyFile.close();
        std::cout << "Secret key serialized successfully." << std::endl;
        
        std::cout << "Keys and crypto context generated and saved successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during serialization: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}