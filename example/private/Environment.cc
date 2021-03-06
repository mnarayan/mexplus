/** Demonstration of mexplus library.
 *
 * This file demonstrates custom data conversion by template specialization.
 *
 * Note that MEX_DEFINE() macro can appear in multiple files.
 *
 */

#include <mexplus/arguments.h>
#include <mexplus/dispatch.h>

using namespace std;
using namespace mexplus;

/** Hypothetical environment object.
 */
struct Environment {
  int code;
  string status;
};

namespace mexplus {

/** Demonstrates custom conversion for write.
 */
template<>
mxArray* MxArray::from(const Environment& value) {
  const char* fields[] = {"code", "status"};
  MxArray struct_array(MxArray::Struct(2, fields));
  struct_array.set("code", value.code);
  struct_array.set("status", value.status);
  return struct_array.release();
}

/** Demonstrates custom conversion for read.
 */
template<>
void MxArray::to(const mxArray* array, Environment* value) {
  if (!value)
    mexErrMsgTxt("Null pointer exception.");
  MxArray::at(array, "code", &value->code);
  MxArray::at(array, "status", &value->status);
}

} // namespace mexplus

namespace {

// This local variable will be kept between MEX calls until cleared.
Environment environment = {0, "environment looks normal."};

// Defines MEX API for getEnvironment.
MEX_DEFINE(getEnvironment) (int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[]) {
  InputArguments input(nrhs, prhs, 0);
  OutputArguments output(nlhs, &plhs, 1);
  output.set(0, MxArray::from(environment));
}

// Defines MEX API for getEnvironment.
MEX_DEFINE(setEnvironment) (int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[]) {
  InputArguments input(nrhs, prhs, 1);
  OutputArguments output(nlhs, &plhs, 0);
  MxArray::to(input.get(0), &environment);
}

} // namespace
