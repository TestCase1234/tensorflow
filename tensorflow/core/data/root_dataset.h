/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_CORE_DATA_ROOT_DATASET_H_
#define TENSORFLOW_CORE_DATA_ROOT_DATASET_H_

#include "tensorflow/core/framework/dataset.h"
#include "tensorflow/core/framework/model.h"
#include "tensorflow/core/framework/model.pb.h"

namespace tensorflow {
namespace data {

// Dataset transformation responsible for internal tf.data logic such as
// autotuning, applying threading configuration.
class RootDataset : public DatasetBase {
 public:
  struct Params {
    bool autotune = true;
    model::AutotuneAlgorithm autotune_algorithm;
    int64 autotune_cpu_budget = 0;
    int64 autotune_ram_budget = 0;
    int64 max_intra_op_parallelism = 1;
    int64 private_threadpool_size = 0;
  };

  static Status FromOptions(OpKernelContext* ctx, DatasetBase* input,
                            DatasetBase** output);

  ~RootDataset() override;

  const DataTypeVector& output_dtypes() const override;
  const std::vector<PartialTensorShape>& output_shapes() const override;

  int64 Cardinality() const override;
  Status CheckExternalState() const override;
  string DebugString() const override;
  Status InputDatasets(std::vector<const DatasetBase*>* inputs) const override;
  std::unique_ptr<IteratorBase> MakeIteratorInternal(
      const string& prefix) const override;

 protected:
  Status AsGraphDefInternal(SerializationContext* ctx,
                            DatasetGraphDefBuilder* b,
                            Node** output) const override;

 private:
  class Iterator;

  RootDataset(OpKernelContext* ctx, const DatasetBase* input, Params params);

  const DatasetBase* input_;
  const Params params_;
  TraceMeMetadata traceme_metadata_;
};

// Finalizes the `input` dataset, which is expected to be called before the
// dataset is about to be iterated. This can for instance apply static graph
// optimizations or inject internal tf.data transformations responsible for
// autotuning or threading configuration.
Status FinalizeDataset(OpKernelContext* ctx, DatasetBase* input,
                       DatasetBase** output);

}  // namespace data
}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_DATA_ROOT_DATASET_H_
