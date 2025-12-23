# Privacy-Preserving Linear Regression using OpenFHE

## Objective

This project implements **linear regression inference under Fully Homomorphic Encryption (FHE)** using the **OpenFHE** library.  

Specifically:
- The **regression model is trained in plaintext** 
- **Model weights remain plaintext**
- **Inputs and outputs are encrypted ciphertexts**
- The server never sees the user’s raw data This enables **privacy-preserving inference**, a key requirement in sensitive domains such as healthcare, finance, and security.

It enables a client to obtain predictions from a server-side regression model **without revealing the input data**, while keeping the model weights in plaintext.

---

## Overview

This project performs the following steps:

1. Train a linear regression model in plaintext
2. Save trained model weights
3. Encrypt the input feature vector
4. Perform regression inference using encrypted operations
5. Decrypt the final result on the client side

---

## Directory Structure

```text
.
├── CMakeLists.txt           # Build configuration
├── config.json              # OpenFHE parameter configuration
├── keygen.cpp               # Crypto context and key generation
├── linear_regression.h      # Regression function declarations
├── linear_regression.cpp    # Encrypted regression implementation
├── main.cpp                 # End-to-end execution
├── model_params.raw         # Plaintext regression weights
├── README.md

```
---

## Model Details

- **Model Type:** Linear Regression  
- **Training:** Performed in plaintext using a standard machine learning library  
- **Inference:** Executed under Fully Homomorphic Encryption (FHE)  
- **Model Weights:** Stored and used in plaintext  
- **Input / Output:** Encrypted ciphertexts  

The trained regression model computes a weighted sum of input features followed by a bias term.  
During inference, the same computation is reproduced using encrypted arithmetic operations provided by OpenFHE.

---

## Cryptographic Scheme

- **Library:** OpenFHE  
- **Scheme:** CKKS (Approximate Arithmetic)  
- **Operations Used:**
  - Ciphertext–Plaintext Multiplication  
  - Ciphertext–Ciphertext Addition  

The CKKS scheme enables efficient evaluation of real-valued linear regression under encryption.

---

## Build Instructions

### Requirements
- C++17
- OpenFHE
- CMake

### Build
```bash
mkdir build
cd build
cmake ..
make
```

### How to Run
```
./fhe_regression
```

The program performs the following:
Generates cryptographic keys
Encrypts the input vector
Executes encrypted regression inference
Decrypts and prints the prediction

-----
## FHE Implementation

The regression inference is implemented using the **OpenFHE** library.  
The following steps are performed:

1. Define the cryptographic context and parameters  
2. Generate public, secret, and evaluation keys  
3. Encode and encrypt the input feature vector  
4. Load regression weights and encode them as plaintexts  
5. Perform ciphertext–plaintext multiplications  
6. Accumulate results using ciphertext additions  
7. Decrypt the final ciphertext to obtain the prediction  

All intermediate computations remain encrypted.

---
## Verification

Correctness is verified by comparing:
Plaintext regression output
Decrypted output from FHE inference
Both results closely match, validating the encrypted computation.

----

## Results

After validation on the FHERMA competition platform, the encrypted regression model achieved a final score of 65.
This confirms the correctness of the FHE-based implementation and its successful integration.

