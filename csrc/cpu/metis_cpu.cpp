#include "metis_cpu.h"

#ifdef WITH_METIS
#include <metis.h>
#endif

#include "utils.h"

torch::Tensor partition_cpu(torch::Tensor rowptr, torch::Tensor col,int64_t num_parts,
                            torch::optional<torch::Tensor> edge_wgt,  bool recursive) {
#ifdef WITH_METIS
  CHECK_CPU(rowptr);
  CHECK_CPU(col);

  int64_t nvtxs = rowptr.numel() - 1;

  auto part = torch::empty(nvtxs, rowptr.options());
  auto *xadj = rowptr.data_ptr<int64_t>();
  auto *adjncy = col.data_ptr<int64_t>();

  int64_t * adjwgt;
  if (edge_wgt.has_value()){
      adjwgt=edge_wgt.value().data_ptr<int64_t>();
      adjwgt=(idx_t*) adjwgt;
  }else{
       adjwgt=nullptr;
  }

  int64_t ncon = 1;
  int64_t objval = -1;
  auto part_data = part.data_ptr<int64_t>();

  if (recursive) {
    METIS_PartGraphRecursive(&nvtxs, &ncon, xadj, adjncy, NULL, NULL, adjwgt,
                             &num_parts, NULL, NULL, NULL, &objval, part_data);
  } else {
    METIS_PartGraphKway(&nvtxs, &ncon, xadj, adjncy, NULL, NULL, adjwgt,
                        &num_parts, NULL, NULL, NULL, &objval, part_data);
  }

  return part;
#else
  AT_ERROR("Not compiled with METIS support");
#endif
}