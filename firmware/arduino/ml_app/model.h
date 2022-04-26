#ifndef MODEL_H
#define MODEL_H

#include <TensorFlowLite.h>
#include "model_tflite.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

class Model {
public:
    Model();
    ~Model();

    void init();
    void draw_pixel(int16_t x, int16_t y);
    void draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void clear_input();
    float execute();

private:
    void swap_int16(int16_t *a, int16_t *b);

    tflite::ErrorReporter* m_error_reporter = nullptr;
    const tflite::Model* m_model = nullptr;
    tflite::MicroInterpreter* m_interpreter = nullptr;
    TfLiteTensor* m_input = nullptr;
    TfLiteTensor* m_output = nullptr;
};

#endif
