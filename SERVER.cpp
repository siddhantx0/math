#include SERVER.hpp
// created by siddhantsingh on 8/4/2023
#include "crow_all.h"        // header + link
#include <curl/curl.h>       // header + link
#include <nlohmann/json.hpp> // header + link

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <thread>
#include <chrono>
#include <filesystem>

enum WindowTypes
{
    code,
    abi
};

class Payload
{
public:
    int codeSize;
    int abiSize;
    std::vector<std::string> codeWindow;
    std::vector<std::string> abiWindow;
};

// function declarations (pre-body)
// getting etherscan + calling/storing windows.
std::string URL(const std::string address);
size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *s);
crow::json::wvalue callEtherScanAPI(const std::string address);
// pre-loading contracts and windows.
void loadContracts();
void loadContractWindows();
// formatting, generating windows.
std::string eraseDubiousElements(std::string s);
std::vector<std::string> generateWindows(const std::string formattedContractString);
// comparison...
std::vector<std::string> loadScamAddresses();
Payload getInformation(std::string, std::unordered_set<WindowTypes>);
// std::vector<std::vector<double>> scamComparison(std::string address);
std::tuple<std::vector<std::string>, std::vector<double>, std::vector<double>> scamComparison(std::string address);

// constants
const std::string ETHERSCAN_API_KEY = "2GXSIQDHS2T4Y1KEKQYCT5PYEAMXNNERR4";
const std::string DIRECTORY = "tokens/";
const std::string ALL_SCAMS = "/Users/siddhantsingh/Documents/Github/tarzan-similarity-server-cpp/tarzan-similarity-server-cpp-x/tarzan-similarity-server-cpp-x/scams.txt";
const std::string SCAMS = "/Users/siddhantsingh/Documents/Github/tarzan-similarity-server-cpp/tarzan-similarity-server-cpp-x/tarzan-similarity-server-cpp-x/scams_shortened.txt";

const size_t WINDOW_SIZE = 5;
const int N = 3; // number of MOST and LEAST similar elements to take relative to a token for comparison...

// methods
std::string URL(std::string address)
{
    return "https://api.etherscan.io/api?module=contract&action=getsourcecode&address=" + address + "&apikey=" + ETHERSCAN_API_KEY;
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size * nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch (std::bad_alloc &e)
    {
        return 0;
    }
    std::copy((char *)contents, (char *)contents + newLength, s->begin() + oldLength);
    return size * nmemb;
}

void createFile(std::string path, std::vector<std::string> store)
{
    std::ofstream file(path);
    for (auto &i : store)
        file << i << " ";
    file.close();
}

std::tuple<std::vector<std::string>, std::vector<double>, std::vector<double>> scamComparison(std::string address)
{
    std::vector<double> outC;
    std::vector<double> outA;
    std::vector<std::string> scams = loadScamAddresses();

    auto e = callEtherScanAPI(address); // store the contract's details :D
    auto infoFlags = std::unordered_set<WindowTypes>{WindowTypes::code, WindowTypes::abi};

    auto start = std::chrono::high_resolution_clock::now();

    Payload thisContract = getInformation(address, infoFlags); // get both types...

    for (auto &contract : scams)
    {
        Payload otherContract = getInformation(contract, infoFlags);

        // do iteration for code
        double codeCount = 0;
        for (int i = 0; i < std::min(thisContract.codeSize, otherContract.codeSize); ++i)
            if (thisContract.codeWindow[i] == otherContract.codeWindow[i])
                codeCount += 1;
        codeCount /= std::min(thisContract.codeSize, otherContract.codeSize);
        outC.emplace_back(codeCount);
        // do iteration for abi
        double abiCount = 0;
        for (int i = 0; i < std::min(thisContract.abiSize, otherContract.abiSize); ++i)
            if (thisContract.abiWindow[i] == otherContract.abiWindow[i])
                abiCount += 1;
        abiCount /= std::min(thisContract.abiSize, otherContract.abiSize);
        outA.emplace_back(abiCount);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << duration.count() << std::endl; // 12 seconds, 10 seconds, 11.7, 9.8
    return std::make_tuple(scams, outC, outA);
}

Payload getInformation(std::string address, std::unordered_set<WindowTypes> getting)
{
    std::string dirPath = DIRECTORY + address;
    std::string infoPath = dirPath + "/info.txt";
    std::string abiPath = dirPath + "/windowsABI.txt";
    std::string codePath = dirPath + "/windowsCode.txt";
    bool getCodeWindow = getting.find(WindowTypes(code)) != getting.end();
    bool getAbiWindow = getting.find(WindowTypes(abi)) != getting.end();

    Payload out;

    std::ifstream infoFile(infoPath);
    std::string s1, s2;
    if (infoFile >> s1 >> s2)
    {
        out.codeSize = std::stoi(s1);
        out.abiSize = std::stoi(s2);
    }

    if (getCodeWindow)
    {
        std::ifstream codeFile(codePath);
        int windowSZ = out.codeSize;
        while (windowSZ-- > 0)
        {
            std::string s;
            codeFile >> s;
            out.codeWindow.emplace_back(s);
        }
    }

    if (getAbiWindow)
    {
        std::ifstream abiFile(abiPath);
        int windowSZ = out.abiSize;
        while (windowSZ-- > 0)
        {
            std::string s;
            abiFile >> s;
            out.abiWindow.emplace_back(s);
        }
    }

    return out;
}

crow::json::wvalue callEtherScanAPI(std::string address)
{
    std::string dirPath = DIRECTORY + address;
    std::string filePath = dirPath + "/response.json";
    nlohmann::json json;
    bool successFlag = false;
    if (std::filesystem::exists(filePath))
    {
        std::ifstream file(filePath);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // get the content of the file...
        file.close();
        try
        {
            json = nlohmann::json::parse(content);
            successFlag = true;
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "JSON parse error: " << e.what() << '\n';
        }
    }
    {
        // File does not exist, perform API call
        CURL *curl;
        CURLcode res;
        std::string readBuffer;
        curl = curl_easy_init();
        if (curl)
        {
            std::string url = URL(address);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            // Check for errors
            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            // always cleanup
            curl_easy_cleanup(curl);
            // Parse JSON
            try
            {
                json = nlohmann::json::parse(readBuffer);
                successFlag = true;
                std::filesystem::create_directories(dirPath);
                std::ofstream file(filePath);
                file << json.dump();
                file.close();
                // saved the json to disk...
            }
            catch (std::runtime_error &e)
            {
                std::cerr << "JSON parse error: " << e.what() << '\n';
            }
        }
    }
    if (successFlag)
    {
        std::string sourceCode = eraseDubiousElements(json["result"][0]["SourceCode"]); // get the source code.
        std::string sourceABI = eraseDubiousElements(json["result"][0]["ABI"]);         // get the source ABI.
        auto windowsCode = generateWindows(sourceCode);                                 // generates the same windows :O
        auto windowsABI = generateWindows(sourceABI);                                   // generates the same windows :O
        // Write windows to a file
        // code windows:
        std::string windowsCodeFilePath = dirPath + "/windowsCode.txt";
        createFile(windowsCodeFilePath, windowsCode);
        // abi windows:
        std::string windowsABIFilePath = dirPath + "/windowsABI.txt";
        createFile(windowsABIFilePath, windowsABI);
        // storing more information
        std::string infoFilePath = dirPath + "/info.txt";
        // windowCode size, windowABI size
        createFile(infoFilePath, std::vector<std::string>{std::to_string(windowsCode.size()), std::to_string(windowsABI.size())});

        return crow::json::load(json.dump());
    }
    return crow::json::wvalue(); // else return NULL json...
}

void run(int port)
{
    crow::SimpleApp app;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CROW_ROUTE(app, "/")
    ([](const crow::request &req) { // take in a request
        crow::response res;         // output a response
        res.body = "hello, internet! siddhant here.";
        return res;
    });
    CROW_ROUTE(app, "/similarity-check")
    ([](const crow::request &req)
     {
        std::string tokenAddress = req.url_params.get("token");
        crow::json::wvalue res;
        if (tokenAddress.empty()) {
            res["error"] = "Missing token parameter";
            return res;
        }
        
        // ADD IN COMPONENTS HERE, SO WE CAN LOAD OUT FROM CACHE RATHER THAN COMPUTATION...
        std::string dirPath = DIRECTORY + tokenAddress;
        std::string path = dirPath + "/similarity.txt";
        std::ifstream infoFile(path);
        std::tuple<std::vector<std::string>, std::vector<double>, std::vector<double>> out;
        int cnt;
        
        if (infoFile >> cnt) {
            // Skip the rest of the first line after reading cnt
            infoFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            for (int i = 0; i < cnt; ++i) {
                std::string line, tokenAddressInFile;
                std::getline(infoFile, line);
                std::istringstream iss(line);
                double code, abi;

                iss >> tokenAddressInFile >> code >> abi;

                std::get<0>(out).push_back(tokenAddressInFile);
                std::get<1>(out).push_back(code);
                std::get<2>(out).push_back(abi);
            }
        }
        else {
            out = scamComparison(tokenAddress);
            std::ofstream outFile(path);
            if (outFile.is_open()) {
                // Write the count of tuples first
                outFile << std::get<0>(out).size() << std::endl;

                // Write the tuples
                for (size_t i = 0; i < std::get<0>(out).size(); ++i) {
                    outFile << std::get<0>(out)[i] << " " << std::get<1>(out)[i] << " " << std::get<2>(out)[i] << std::endl;
                }

                // Close the file
                outFile.close();
            } else {
                std::cerr << "Could not create file: " << path << std::endl;
            }
        }

        // to get elements from a tuple...
        auto [scams, outC, outA] = out;
        std::vector<std::tuple<std::string, double>> C;
        std::vector<std::tuple<std::string, double>> A;
        for (size_t i = 0; i < scams.size(); ++i) {
            std::cout << scams[i] << " " << outC[i] << " " << outA[i] << "\n";
            C.push_back(std::make_tuple(scams[i], outC[i]));
            A.push_back(std::make_tuple(scams[i], outA[i]));
        }
        // if i were to sort outC --> i could retrieve the top three and bottom three components
        
        std::sort(C.begin(), C.end(), [](const auto &a, const auto &b){return std::get<1>(a) < std::get<1>(b);});
        std::sort(A.begin(), A.end(), [](const auto &a, const auto &b){return std::get<1>(a) < std::get<1>(b);});
        
        std::vector<std::tuple<std::string, double>> mostSimilarCode3;
        std::vector<std::tuple<std::string, double>> leastSimilarCode3;
        std::vector<std::tuple<std::string, double>> mostSimilarABI3;
        std::vector<std::tuple<std::string, double>> leastSimilarABI3;
        
        std::copy(C.end() - N, C.end(), std::back_inserter(mostSimilarCode3));
        std::copy(C.begin(), C.begin() + N, std::back_inserter(leastSimilarCode3));
        
        std::copy(A.end() - N, A.end(), std::back_inserter(mostSimilarABI3));
        std::copy(A.begin(), A.begin() + N, std::back_inserter(leastSimilarABI3));
        
        int i = 0;
        for (const auto&[address, similarity] : mostSimilarCode3) {
            res["most-similar-code"][i]["address"] = address;
            res["most-similar-code"][i]["similarity"] = similarity;
            ++i;
        }
        i = 0;
        for (const auto&[address, similarity] : leastSimilarCode3) {
            res["least-similar-code"][i]["address"] = address;
            res["least-similar-code"][i]["similarity"] = similarity;
            ++i;
        }
        i = 0;
        for (const auto&[address, similarity] : mostSimilarABI3) {
            res["most-similar-abi"][i]["address"] = address;
            res["most-similar-abi"][i]["similarity"] = similarity;
            ++i;
        }
        i = 0;
        for (const auto&[address, similarity] : leastSimilarABI3) {
            res["least-similar-abi"][i]["address"] = address;
            res["least-similar-abi"][i]["similarity"] = similarity;
            ++i;
        }
        
        return res; });

    std::cout << "starting crow." << std::endl;
    app.port(port).run_async();
    curl_global_cleanup();
}

std::vector<std::string> loadScamAddresses()
{
    auto loader = ALL_SCAMS; // ALL_SCAMS works as well...
    std::ifstream inputFile(loader);

    if (!inputFile.is_open())
    {
        std::cerr << "Error: Failed to open " << loader << std::endl;
        return std::vector<std::string>();
    }

    std::string address;
    std::vector<std::string> addresses;

    while (getline(inputFile, address))
        addresses.push_back(address);

    inputFile.close();
    return addresses;
}

void loadContracts()
{
    // take in a text file of addresses. will be in the same directory and called "scams.txt" cin << each address and load with the "callEtherScanAPI" method...
    // the text file will have tokens line by line...
    // make sure to keep track of the number of API requests, should not be more than 5 per second.
    std::vector<std::string> addresses = loadScamAddresses();

    const int maxRequestsPerSecond = 5;
    const int millisecondsPerSecond = 1000;
    const int millisecondsPerRequest = millisecondsPerSecond / maxRequestsPerSecond;

    int requestCount = 0;
    int total = 0;
    for (const auto &address : addresses)
    {
        if (requestCount >= maxRequestsPerSecond)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsPerRequest));
            requestCount = 0;
        }
        crow::json::wvalue result = callEtherScanAPI(address);
        requestCount++;
        std::cout << ++total << ", " << address << std::endl;
    }
}

std::vector<std::string> generateWindows(std::string formattedContractString)
{
    std::vector<std::string> windows = std::vector<std::string>();
    size_t sz = formattedContractString.size();
    for (size_t i = 0; i + WINDOW_SIZE < sz + 1; i++)
    {
        std::string curr = formattedContractString.substr(i, WINDOW_SIZE);
        windows.emplace_back(curr);
    }
    return windows;
}

std::string eraseDubiousElements(std::string s)
{
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c)
                           { return std::isspace(c); }),
            s.end());
    return s;
}

int main(int argc, const char *argv[])
{
    run(18080);
    return 0;
}

/*
 void run(int port) {
     // create the TokenVector object + thread for checking info...
     // app - organizes parts of crow and creates an interface to interact with
     // allows access to HTTP server
     // router
     // middlewares
     // two types: SimpleApp and App<m1, m2, ...>; App allows different types of middleware
     crow::SimpleApp app;
     curl_global_init(CURL_GLOBAL_DEFAULT);
     // CROW_ROUTE is a macro for "#define CROW_ROUTE(app, url) app.template route<crow::black_magic::get_parameter_tag(url)>(url)"
     CROW_ROUTE(app, "/")([](const crow::request& req){ // take in a request
         crow::response res; // output a response
         res.body = "hello, internet! siddhant here.";
         return res;
     });

     // this is a lambda function, you need to capture tv by reference by using &tv...
     CROW_ROUTE(app, "/similarity-check")([](const crow::request& req) {
         std::string tokenAddress = req.url_params.get("token");
         // &tv
         if (tokenAddress.empty()) {
             // Return an error if the token parameter is not provided
             crow::json::wvalue res;
             res["error"] = "Missing token parameter";
             return res;
         }
 //      Coroutines: https://youtu.be/kIPzED3VD3w
 //      Concurrency: https://youtu.be/pfIC-kle4b0
 //      Multithreading: https://youtu.be/A7sVFJLJM-A
 //      Threads/Thread-pools/C: https://youtu.be/d9s_d28yJq0?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
         auto out = scamComparison(tokenAddress);
         // to get elements from a tuple...
         std::vector<size_t> idxC(std::get<1>(out).size());
         std::vector<size_t> idxA(std::get<2>(out).size()); // different similarity scores for ABI as compared to Code.
         // sort...
         std::sort(idxC.begin(), idxC.end(), [&out](size_t i1, size_t i2) {return std::get<1>(out)[i1] < std::get<1>(out)[i2]; });
         std::sort(idxA.begin(), idxA.end(), [&out](size_t i1, size_t i2) {return std::get<2>(out)[i1] < std::get<2>(out)[i2]; });

         std::vector<std::string> sortedScamsByC(std::get<1>(out).size());
         std::vector<double> sortedOutC(std::get<1>(out).size());
         for (size_t i = 0; i < idxC.size(); ++i) {
             sortedScamsByC[i] = std::get<0>(out)[idxC[i]];
             sortedOutC[i] = std::get<1>(out)[idxC[i]];
         }

         std::vector<std::string> sortedScamsByA(std::get<2>(out).size());
         std::vector<double> sortedOutA(std::get<2>(out).size());
         for (size_t i = 0; i < idxA.size(); ++i) {
             sortedScamsByA[i] = std::get<0>(out)[idxA[i]];
             sortedOutA[i] = std::get<2>(out)[idxA[i]];
         }

 //        auto maxCode = std::max_element(out[0].begin(), out[0].end());
 //        auto maxAbi = std::max_element(out[1].begin(), out[1].end());
 ////        std::cout << "max code: " << *maxCode << " | max abi: " << *maxAbi << std::endl;
 //        std::cout << tokenAddress << ": (" << *maxCode << ", " << *maxAbi << ").\n";

         // Extract the top 3 and bottom 3 code contracts and scores
         std::vector<std::string> top3CodeContracts = {sortedScamsByC[0], sortedScamsByC[1], sortedScamsByC[2]};
         std::vector<double> top3CodeScores = {sortedOutC[0], sortedOutC[1], sortedOutC[2]};

         size_t end = std::get<2>(out).size();

         std::vector<std::string> bottom3CodeContracts = {sortedScamsByC[end-3], sortedScamsByC[end-2], sortedScamsByC[end-1]};
         std::vector<double> bottom3CodeScores = {sortedOutC[end-3], sortedOutC[end-2], sortedOutC[end-1]};

         // Extract the top 3 and bottom 3 ABI contracts and scores
         std::vector<std::string> top3AbiContracts = {sortedScamsByA[0], sortedScamsByA[1], sortedScamsByA[2]};
         std::vector<double> top3AbiScores = {sortedOutA[0], sortedOutA[1], sortedOutA[2]};

         std::vector<std::string> bottom3AbiContracts = {sortedScamsByA[end-3], sortedScamsByA[end-2], sortedScamsByA[end-1]};
         std::vector<double> bottom3AbiScores = {sortedOutA[end-3], sortedOutA[end-2], sortedOutA[end-1]};

         // Add this information to the res JSON object using crow::json
         try {
             crow::json::wvalue res;
             res["token"] = tokenAddress;

             for (int i = 0; i < 3; ++i) {
                 res["max-code-3"][i]["contract"] = top3CodeContracts[i];
                 res["max-code-3"][i]["score"] = top3CodeScores[i];

                 res["min-code-3"][i]["contract"] = bottom3CodeContracts[i];
                 res["min-code-3"][i]["score"] = bottom3CodeScores[i];

                 res["max-abi-3"][i]["contract"] = top3AbiContracts[i];
                 res["max-abi-3"][i]["score"] = top3AbiScores[i];

                 res["min-abi-3"][i]["contract"] = bottom3AbiContracts[i];
                 res["min-abi-3"][i]["score"] = bottom3AbiScores[i];
             }

             return res;
         } catch (const std::exception& e) {
             crow::json::wvalue res;
             res["error"] = std::string("An error occurred: ") + e.what();
             return res;
         }

 //
 //        try {
 //            crow::json::wvalue res;
 //            res["token"] = tokenAddress;
 //            res["max-code-3"] =
 //            res["min-code-3"] =
 //            res["max-abi-3"] =
 //            res["min-code-3"] =
 ////            res["vector-code"] = out[0];
 ////            res["vector-abi"] = out[1];
 ////            auto start = std::chrono::high_resolution_clock::now();
 ////            res["output"] = callEtherScanAPI(tokenAddress);
 ////            auto stop = std::chrono::high_resolution_clock::now();
 ////            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 //
 ////            res["duration"] = duration.count();
 //            return res;
 //        } catch (const std::exception& e) {
 //            crow::json::wvalue res;
 //            res["error"] = std::string("An error occurred: ") + e.what();
 //            return res;
 //        }
     });

     std::cout << "starting crow." << std::endl;
     app.port(port).run_async();
     curl_global_cleanup();
 }
 */
