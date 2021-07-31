#pragma once

#include <vector>
#include "dl_constant.hpp"
#include "dl_variable.hpp"
#include "dl_nn_global_max_pool2d.hpp"

namespace dl
{
    namespace layer
    {
        /**
         * @brief GlobalMaxPool2D(input).
         * 
         * @tparam feature_t supports int16_t and int8_t,
         *         - int16_t: stands for operation in int16_t quantize
         *         - int8_t: stands for operation in int8_t quantize
         */
        template <typename feature_t>
        class GlobalMaxPool2D : public Layer
        {
        public:
            Tensor<feature_t> output; /*<! output of GlobalMaxPool2D >*/

            /**
             * @brief Construct a new GlobalMaxPool2D object.
             * 
             * @param name         name of layer
             */
            GlobalMaxPool2D(const std::vector<int> filter_shape,
                            const char *name = NULL) : Layer(name) {}

            /**
             * @brief Destroy the GlobalMaxPool2D object.
             * 
             */
            ~GlobalMaxPool2D() {}

            /**
             * @brief Update output shape and exponent.
             * 
             * @param input as an input
             */
            void build(Tensor<feature_t> &input)
            {
                assert(input.shape[0] > 0);
                assert(input.shape[1] > 0);
                this->output.set_exponent(input.exponent);

                vector<int> output_shape(input.shape.size(), 1);
                output_shape[2] = input.shape[2];
                this->output.set_shape(output_shape);
            }

            /**
             * @brief Call GlobalMaxPool2D operation
             * 
             * @param input           as an input
             * @param autoload_enable one of true or false, 
             *                        - true: load input and output from PSRAM to CACHE automatically
             *                        - false: do not
             * @param assign_core     not effective yet
             * @return GlobalMaxPool2D result
             */
            Tensor<feature_t> &call(Tensor<feature_t> &input, uint8_t autoload_enable = 0)
            {
                DL_LOG_LAYER_LATENCY_INIT();

                DL_LOG_LAYER_LATENCY_START();
                this->output.apply_element();
                DL_LOG_LAYER_LATENCY_END(this->name, "apply");

                if (autoload_enable)
                {
                    dl::tool::cache::autoload_func((uint32_t)(this->output.element), this->output.get_size() * sizeof(feature_t),
                                                   (uint32_t)(input.element), input.get_size() * sizeof(feature_t));
                }

                DL_LOG_LAYER_LATENCY_START();
                nn::global_max_pool2d(output, input);
                DL_LOG_LAYER_LATENCY_END(this->name, "global_max_pool2d");

                return this->output;
            }
        };
    } // namespace layer
} // namespace dl
