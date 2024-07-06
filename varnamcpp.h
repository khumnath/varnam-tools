#ifndef VARNAMCPP_H
#define VARNAMCPP_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
extern "C" {
#include <libgovarnam/libgovarnam.h>
}

using namespace std;

struct CSuggestion {
    string word;
    int weight;
    int learnedOn;

    CSuggestion(string w, int wt, int lo) : word(w), weight(wt), learnedOn(lo) {}
};

class VarnamHandler {
public:
    VarnamHandler(const string& schemeID) {
        int err = varnam_init_from_id(const_cast<char*>(schemeID.c_str()), &v);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to initialize Varnam: " + string(varnam_get_last_error(v)));
        }
    }

    ~VarnamHandler() {
        varnam_close(v);
    }

    void setDebug(bool debug) {
        varnam_debug(v, debug);
    }

    void updateVarnamConfig(bool enableIndicNumbers,int dictSuggestionsLimit, int patternDictSuggestionsLimit, int tokenizerSuggestionsLimit, bool ignoreDuplicateToken, int dictionaryMatchExact) {

        int err = varnam_config(v, VARNAM_CONFIG_USE_INDIC_DIGITS, enableIndicNumbers ? 1 : 0);
        if (err != VARNAM_SUCCESS) {
            throw std::runtime_error("Failed to set Indic numbers configuration: " + std::string(varnam_get_last_error(v)));
        }

         err = varnam_config(v, VARNAM_CONFIG_SET_DICTIONARY_SUGGESTIONS_LIMIT, dictSuggestionsLimit);
        if (err != VARNAM_SUCCESS) {
            throw std::runtime_error("Failed to set dictionary suggestions limit: " + std::string(varnam_get_last_error(v)));
        }

        err = varnam_config(v, VARNAM_CONFIG_SET_PATTERN_DICTIONARY_SUGGESTIONS_LIMIT, patternDictSuggestionsLimit);
        if (err != VARNAM_SUCCESS) {
            throw std::runtime_error("Failed to set pattern dictionary suggestions limit: " + std::string(varnam_get_last_error(v)));
        }

        err = varnam_config(v, VARNAM_CONFIG_SET_TOKENIZER_SUGGESTIONS_LIMIT, tokenizerSuggestionsLimit);
        if (err != VARNAM_SUCCESS) {
            throw std::runtime_error("Failed to set tokenizer suggestions limit: " + std::string(varnam_get_last_error(v)));
        }

        err = varnam_config(v, VARNAM_CONFIG_IGNORE_DUPLICATE_TOKEN, ignoreDuplicateToken);
        if (err != VARNAM_SUCCESS) {
            throw std::runtime_error("Failed to ignore duplicate tokens: " + std::string(varnam_get_last_error(v)));
        }

        err = varnam_config(v, VARNAM_CONFIG_SET_DICTIONARY_MATCH_EXACT, dictionaryMatchExact);
        if (err != VARNAM_SUCCESS) {
            throw std::runtime_error("Failed to set dict suggestions always: " + std::string(varnam_get_last_error(v)));
        }
    }

/* number config was first di
   void enableIndicNumbers(bool enable) {
        int err;
        if (enable) {
            err = varnam_config(v, VARNAM_CONFIG_USE_INDIC_DIGITS, 1);
        } else {
            err = varnam_config(v, VARNAM_CONFIG_USE_INDIC_DIGITS, 0);
        }
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to set Indic numbers configuration: " + string(varnam_get_last_error(v)));
        }
    }*/

    void reIndexDictionary() {
        int err = varnam_reindex_dictionary(v);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to re-index dictionary: " + string(varnam_get_last_error(v)));
        }
        cout << "Re-indexed dictionary." << endl;
    }

    void train(const string& pattern, const string& word) {
        int err = varnam_train(v, const_cast<char*>(pattern.c_str()), const_cast<char*>(word.c_str()));
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to train pattern '" + pattern + "' => word '" + word + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Trained " << pattern << " => " << word << endl;
    }

    void learn(const string& word) {
        int err = varnam_learn(v, const_cast<char*>(word.c_str()), 0); // Assuming default weight as 0
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to learn word '" + word + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Learnt " << word << endl;
    }

    void unlearn(const string& word) {
        int err = varnam_unlearn(v, const_cast<char*>(word.c_str()));
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to unlearn word '" + word + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Unlearnt " << word << endl;
    }

    void learnFromFile(const string& filePath) {
        LearnStatus* status;
        int err = varnam_learn_from_file(v, const_cast<char*>(filePath.c_str()), &status);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to learn words from file '" + filePath + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Finished learning words: " << status->TotalWords << ". Failed: " << status->FailedWords << endl;
    }

    void trainFromFile(const string& filePath) {
        LearnStatus* status;
        int err = varnam_train_from_file(v, const_cast<char*>(filePath.c_str()), &status);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to train words from file '" + filePath + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Finished training words: " << status->TotalWords << ". Failed: " << status->FailedWords << endl;
    }

    void export_(const string& filePath, int exportWordsPerFile) {
        int err = varnam_export(v, const_cast<char*>(filePath.c_str()), exportWordsPerFile);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to export learnings to file '" + filePath + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Finished exporting learnings." << endl;
    }

    void import(const string& filePath) {
        int err = varnam_import(v, const_cast<char*>(filePath.c_str()));
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to import learnings from file '" + filePath + "': " + string(varnam_get_last_error(v)));
        }
        cout << "Finished importing learnings from file '" + filePath + "'." << endl;
    }

    vector<CSuggestion> reverseTransliterate(const string& text) {
        varray* result;
        int err = varnam_reverse_transliterate(v, const_cast<char*>(text.c_str()), &result);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to reverse transliterate '" + text + "': " + string(varnam_get_last_error(v)));
        }
        return convertVarrayToSuggestions(result);
    }

    vector<CSuggestion> transliterate(const string& text) {
        varray* result;
        int err = varnam_transliterate(v, 0, const_cast<char*>(text.c_str()), &result);
        if (err != VARNAM_SUCCESS) {
            throw runtime_error("Failed to transliterate '" + text + "': " + string(varnam_get_last_error(v)));
        }
        return convertVarrayToSuggestions(result);
    }

private:
    int v;

    vector<CSuggestion> convertVarrayToSuggestions(varray* arr) {
        vector<CSuggestion> result;
        for (int i = 0; i < varray_length(arr); i++) {
            Suggestion_t* sug = static_cast<Suggestion_t*>(varray_get(arr, i));
            result.emplace_back(string(sug->Word), sug->Weight, sug->LearnedOn);
        }
        varray_free(arr, free);
        return result;
    }
};

#endif // VARNAMCPP_H
