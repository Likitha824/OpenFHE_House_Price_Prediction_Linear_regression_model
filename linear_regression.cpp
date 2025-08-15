#include "linear_regression.h"
#include <fstream>
#include <iostream>

CKKSTaskSolver::CKKSTaskSolver(std::string ccLocation, std::string pubKeyLocation, std::string multKeyLocation,
                         std::string rotKeyLocation,
                         std::string inputLocation,
                         std::string outputLocation) : m_PubKeyLocation(pubKeyLocation),
                                                       m_MultKeyLocation(multKeyLocation),
                                                       m_RotKeyLocation(rotKeyLocation),
                                                       m_CCLocation(ccLocation),
                                                       m_InputLocation(inputLocation),
                                                       m_OutputLocation(outputLocation)
{
    initCC();
    loadModelParameters();
};

void CKKSTaskSolver::initCC()
{
    if (!Serial::DeserializeFromFile(m_CCLocation, m_cc, SerType::BINARY))
    {
        std::cerr << "Could not deserialize cryptocontext file" << std::endl;
        std::exit(1);
    }

    if (!Serial::DeserializeFromFile(m_PubKeyLocation, m_PublicKey, SerType::BINARY))
    {
        std::cerr << "Could not deserialize public key file" << std::endl;
        std::exit(1);
    }

    std::ifstream multKeyIStream(m_MultKeyLocation, std::ios::in | std::ios::binary);
    if (!multKeyIStream.is_open())
    {
        std::cerr << "Could not open multiplication key file" << std::endl;
        std::exit(1);
    }
    if (!m_cc->DeserializeEvalMultKey(multKeyIStream, SerType::BINARY))
    {
        std::cerr << "Could not deserialize multiplication key file" << std::endl;
        std::exit(1);
    }

    std::ifstream rotKeyIStream(m_RotKeyLocation, std::ios::in | std::ios::binary);
    if (!rotKeyIStream.is_open())
    {
        std::cerr << "Could not open rotation key file" << std::endl;
        std::exit(1);
    }

    if (!m_cc->DeserializeEvalAutomorphismKey(rotKeyIStream, SerType::BINARY))
    {
        std::cerr << "Could not deserialize rotation key file" << std::endl;
        std::exit(1);
    }

    if (!Serial::DeserializeFromFile(m_InputLocation, m_InputC, SerType::BINARY))
    {
        std::cerr << "Could not deserialize Input ciphertext" << std::endl;
        std::exit(1);
    }
}

// Helper to encode vectors as plaintexts
Plaintext CKKSTaskSolver::encode(const std::vector<double> &vec, int level) {
    Plaintext p = m_cc->MakeCKKSPackedPlaintext(vec, 1, level);
    p->SetLength(vec.size());
    return p;
}

void CKKSTaskSolver::loadModelParameters() 
{
    std::ifstream file(m_WeightsFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open weights file: " << m_WeightsFile << std::endl;
        std::exit(1);
    }
    
    int num_weights;
    file.read(reinterpret_cast<char*>(&num_weights), sizeof(int));
    
    m_Weights.resize(num_weights);
    for (int i = 0; i < num_weights; i++) {
        file.read(reinterpret_cast<char*>(&m_Weights[i]), sizeof(double));
    }
    
    file.read(reinterpret_cast<char*>(&m_Bias), sizeof(double));
    file.close();
    
    std::cout << "Loaded " << num_weights << " weights and bias: " << m_Bias << std::endl;
}

void CKKSTaskSolver::eval()
{
    // Create plaintext for weights
    Plaintext weightPlaintext = encode(m_Weights, m_InputC->GetLevel());
    
    // Multiply input features by weights (element-wise multiplication)
    auto weightedFeatures = m_cc->EvalMult(m_InputC, weightPlaintext);
    
    // Sum all slots to get dot product result
    auto dotProductResult = m_cc->EvalSum(weightedFeatures, m_Weights.size());
    
    // Create plaintext for bias
    std::vector<double> biasVector = {m_Bias};
    Plaintext biasPlaintext = encode(biasVector, dotProductResult->GetLevel());
    
    // Add bias to get final prediction
    m_OutputC = m_cc->EvalAdd(dotProductResult, biasPlaintext);
    
    std::cout << "✅ House price prediction completed under FHE." << std::endl;
}

void CKKSTaskSolver::serializeOutput()
{
    if (!Serial::SerializeToFile(m_OutputLocation, m_OutputC, SerType::BINARY))
    {
        std::cerr << " Error writing output ciphertext" << std::endl;
    }
}