#include "openfhe.h"

#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"

#include <vector>
#include <string>

using namespace lbcrypto;

class CKKSTaskSolver {
    CryptoContext<DCRTPoly> m_cc;
    PublicKey<DCRTPoly> m_PublicKey;
    Ciphertext<DCRTPoly> m_InputC;
    Ciphertext<DCRTPoly> m_OutputC;
    std::string m_PubKeyLocation;
    std::string m_MultKeyLocation;
    std::string m_RotKeyLocation;
    std::string m_CCLocation;
    std::string m_InputLocation;
    std::string m_OutputLocation;
    std::string m_WeightsFile = "model_params.raw";
    
    // Model parameters
    std::vector<double> m_Weights;
    double m_Bias;
    
    // Helper function to encode vectors as plaintexts
    Plaintext encode(const std::vector<double> &vec, int level);

public:
    CKKSTaskSolver(std::string ccLocation, std::string pubKeyLocation, std::string multKeyLocation,
                std::string rotKeyLocation,
                std::string inputLocation,
                std::string outputLocation);

    void initCC();
    void loadModelParameters();
    void eval();
    void serializeOutput();
};