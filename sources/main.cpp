#include "api_wrapper.h"

void main() {

	language_model the_language_model;
    the_language_model.params.model = "C:/Users/Quill/Documents/ai/models/mistral-7b-v0.1.Q8_0.gguf";
    the_language_model.params.interactive = true;
    the_language_model.params.interactive_first = true;
    the_language_model.params.antiprompt.push_back("User:");//the phrase the model outputs when it passes control back. Some models might output EOS instead
    the_language_model.params.n_gpu_layers = 81;

	the_language_model.initialize();

    std::string input_text;
    std::string output_text;
 
    input_text = "User: Hello, can you tell me the capital of France?\nAssistant:";
    output_text.resize(0);
    the_language_model.send_input_and_receive_output(input_text, output_text);
    printf("\n\n");
    printf("input text: %s\n", input_text.c_str());
    printf("output text: %s\n", output_text.c_str());

    input_text = "Can you tell me the capital of Germany?\nAssistant:";
    output_text.resize(0);
    the_language_model.send_input_and_receive_output(input_text, output_text);
    printf("\n\n");
    printf("input text: %s\n", input_text.c_str());
    printf("output text: %s\n", output_text.c_str());

    input_text = "Can you tell me the first question I asked?\nAssistant:";
    output_text.resize(0);
    the_language_model.send_input_and_receive_output(input_text, output_text);
    printf("\n\n");
    printf("input text: %s\n", input_text.c_str());
    printf("output text: %s\n", output_text.c_str());

    input_text = "Are there any noticeable issues with the input text you are receiving?\nAssistant:";
    output_text.resize(0);
    the_language_model.send_input_and_receive_output(input_text, output_text);
    printf("\n\n");
    printf("input text: %s\n", input_text.c_str());
    printf("output text: %s\n", output_text.c_str());

}//end main


