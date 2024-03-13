#pragma once

#include "common.h"

#include "console.h"
#include "llama.h"

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <signal.h>
#include <unistd.h>
#elif defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <signal.h>
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4244 4267) // possible loss of data
#endif

static llama_context** g_ctx;
static llama_model** g_model;
static gpt_params* g_params;
static std::vector<llama_token>* g_input_tokens;
static std::ostringstream* g_output_ss;
static std::vector<llama_token>* g_output_tokens;
static bool is_interacting = false;

static bool file_exists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

static bool file_is_empty(const std::string& path) {
    std::ifstream f;
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f.open(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    return f.tellg() == 0;
}

static void write_logfile(
    const llama_context* ctx, const gpt_params& params, const llama_model* model,
    const std::vector<llama_token>& input_tokens, const std::string& output,
    const std::vector<llama_token>& output_tokens
) {
    if (params.logdir.empty()) {
        return;
    }

    const std::string timestamp = get_sortable_timestamp();

    const bool success = create_directory_with_parents(params.logdir);
    if (!success) {
        fprintf(stderr, "%s: warning: failed to create logdir %s, cannot write logfile\n",
            __func__, params.logdir.c_str());
        return;
    }

    const std::string logfile_path = params.logdir + timestamp + ".yml";
    FILE* logfile = fopen(logfile_path.c_str(), "w");

    if (logfile == NULL) {
        fprintf(stderr, "%s: failed to open logfile %s\n", __func__, logfile_path.c_str());
        return;
    }

    fprintf(logfile, "binary: main\n");
    char model_desc[128];
    llama_model_desc(model, model_desc, sizeof(model_desc));
    dump_non_result_info_yaml(logfile, params, ctx, timestamp, input_tokens, model_desc);

    fprintf(logfile, "\n");
    fprintf(logfile, "######################\n");
    fprintf(logfile, "# Generation Results #\n");
    fprintf(logfile, "######################\n");
    fprintf(logfile, "\n");

    dump_string_yaml_multiline(logfile, "output", output.c_str());
    dump_vector_int_yaml(logfile, "output_tokens", output_tokens);

    llama_dump_timing_info_yaml(logfile, ctx);
    fclose(logfile);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
static void sigint_handler(int signo) {
    if (signo == SIGINT) {
        if (!is_interacting && g_params->interactive) {
            is_interacting = true;
        }
        else {
            console::cleanup();
            printf("\n");
            llama_print_timings(*g_ctx);
            write_logfile(*g_ctx, *g_params, *g_model, *g_input_tokens, g_output_ss->str(), *g_output_tokens);
            _exit(130);
        }
    }
}
#endif

static void llama_log_callback_logTee(ggml_log_level level, const char* text, void* user_data) {
    (void)level;
    (void)user_data;
    LOG_TEE("%s", text);
}







struct language_model {

    gpt_params params;//model parameters such as file path of model, interactive mode, etc.

    llama_context* ctx;
    int n_ctx;
    int ga_n;
    int n_past;
    int guidance_offset;
    
    llama_context* ctx_guidance = NULL;
    int n_past_guidance = 0;
    std::string path_session;
    // group-attention state
    // number of grouped KV tokens so far (used only if params.grp_attn_n > 1)
    int ga_i = 0;
    int ga_w;
    int n_remain;
    int n_session_consumed = 0;
    std::vector<llama_token> session_tokens;
    std::vector<llama_token> guidance_inp;
    std::vector<llama_token> embd_guidance;
    int original_prompt_len = 0;
    std::vector<llama_token> embd;
    std::vector<llama_token> embd_inp;
    int n_consumed = 0;
    bool need_to_save_session;
    struct llama_sampling_context* ctx_sampling;
    bool input_echo = true;
    bool display = true;
    std::vector<int>   input_tokens;
    std::vector<int>   output_tokens;
    std::ostringstream output_ss;
    llama_model* model;
    std::vector<int> inp_pfx;
    std::vector<int> cml_pfx;
    std::vector<int> inp_sfx;
    std::vector<int> cml_sfx;

	int initialize();

    int send_input_and_receive_output(std::string input_text, std::string& output_text);

    int save_inputs_and_outputs();


};//end neural net