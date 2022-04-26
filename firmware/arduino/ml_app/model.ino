#include <TensorFlowLite.h>
#include "model_tflite.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "model.h"

constexpr uint32_t g_width = 30;
constexpr uint32_t g_height = 30;
constexpr int g_tensor_arena_size = 60000;
uint8_t g_tensor_arena[g_tensor_arena_size];

Model::Model()
{
    m_error_reporter = nullptr;
    m_model = nullptr;
    m_interpreter = nullptr;
    m_input = nullptr;
    m_output = nullptr;
}

Model::~Model()
{

}

void Model::init()
{
    // Setup logger
    static tflite::MicroErrorReporter micro_error_reporter;
    m_error_reporter = &micro_error_reporter;

    // Get the model
    m_model = tflite::GetModel(model_tflite);
    if (m_model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(m_error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             m_model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Setup tensorflow lite model interpreter
    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(m_model, resolver,
                                                       g_tensor_arena, g_tensor_arena_size,
                                                       m_error_reporter);
    m_interpreter = &static_interpreter;

    // Allocate tensors
    TfLiteStatus allocate_status = m_interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(m_error_reporter, "AllocateTensors() failed");
        return;
    }

    // Get pointers to input and output tensors
    m_input = m_interpreter->input(0);
    m_output = m_interpreter->output(0);

    clear_input();
}

void Model::draw_pixel(int16_t x, int16_t y)
{
    x = x/8;
    y = y/8;

    m_input->data.f[y*g_height+x] = 255.0;
}

void Model::swap_int16(int16_t *a, int16_t *b)
{
    int16_t temp_a = *a;
    *a = *b;
    *b = temp_a;
}

void Model::draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap_int16(&x0, &y0);
        swap_int16(&x1, &y1);
    }

    if (x0 > x1) {
        swap_int16(&x0, &x1);
        swap_int16(&y0, &y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            draw_pixel(y0, x0);
        } else {
            draw_pixel(x0, y0);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void Model::clear_input()
{
    for (uint32_t i = 0; i < g_width * g_height; i++) {
        m_input->data.f[i] = 0.0;
    }
}

float Model::execute()
{
    TfLiteStatus invoke_status = m_interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        Serial1.println("FAIL");
        TF_LITE_REPORT_ERROR(m_error_reporter, "Invoke failed\n");
        return -1;
    }

    return m_output->data.f[0];
}
