/** MEX function helper library.
 *
 * Kota Yamaguchi 2013 <kyamagu@cs.stonybrook.edu>
 */

#include <mexplus/arguments.h>
#include <memory>

#define EXPECT(condition) if (!(condition)) \
    mexErrMsgTxt(#condition " not true.")
#define RUN_TEST(function) function(); \
    mexPrintf("PASS: %s\n", #function); \
    mexCallMATLAB(0, NULL, 0, NULL, "drawnow")

using namespace std;
using mexplus::InputArguments;
using mexplus::OutputArguments;

namespace {

// Declare a memory-safe mxArray*. Note that Matlab automatically frees up
// mxArray* when MEX exits unless flagged persistent.
#define MAKE_VALUE(pointer) \
  shared_ptr<mxArray>(pointer, mxDestroyArray)
// Initialize vector<const mxArray*> rhs.
#define MAKE_RHS(rhs, ...) \
  const shared_ptr<mxArray> kFixture[] = { __VA_ARGS__ }; \
  const size_t kFixtureSize = sizeof(kFixture) / sizeof(shared_ptr<mxArray>); \
  vector<const mxArray*> rhs(kFixtureSize); \
  for (int i = 0; i < kFixtureSize; ++i) \
    rhs[i] = kFixture[i].get();

/** Test a single format input with default options.
 */
void testInputsSingleFormatOptionsDefault() {
  MAKE_RHS(
    rhs,
    MAKE_VALUE(mxCreateDoubleScalar(3.2)),
    MAKE_VALUE(mxCreateString("Text input."))
  );
  InputArguments input(rhs.size(), &rhs[0], 2, 2, "Option1", "Option2");
  EXPECT(input.get<double>(0) == 3.2);
  EXPECT(input.get<string>(1) == "Text input.");
  EXPECT(input.get<double>("Option1", -1) == -1);
  EXPECT(input.get<string>("Option2", "Option2 value.") == "Option2 value.");
}

/** Test a single format input with updated options.
 */
void testInputsSingleFormatOptionsUpdate() {
  MAKE_RHS(
    rhs,
    MAKE_VALUE(mxCreateDoubleScalar(3.2)),
    MAKE_VALUE(mxCreateString("Text input.")),
    MAKE_VALUE(mxCreateString("Option2")),
    MAKE_VALUE(mxCreateString("Option2 value.")),
    MAKE_VALUE(mxCreateString("Option1")),
    MAKE_VALUE(mxCreateDoubleScalar(10))
  );
  InputArguments input(rhs.size(), &rhs[0], 2, 2, "Option1", "Option2");
  EXPECT(input.is("default"));
  EXPECT(input.get<double>(0) == 3.2);
  EXPECT(input.get<string>(1) == "Text input.");
  EXPECT(input.get<double>("Option1", -1) == 10);
  EXPECT(input.get<string>("Option2", "Some value.") == "Option2 value.");
}

/** Test a single format input with default options.
 */
void testInputsMultipleFormats() {
  MAKE_RHS(
    rhs,
    MAKE_VALUE(mxCreateDoubleScalar(3.2)),
    MAKE_VALUE(mxCreateString("Text input."))
  );
  InputArguments input;
  input.define("format1", 1, 2, "Option1", "Option2");
  input.define("format2", 2, 2, "Option1", "Option2");
  input.define("format3", 3);
  input.parse(rhs.size(), &rhs[0]);
  EXPECT(!input.is("format1"));
  EXPECT(input.is("format2"));
  EXPECT(!input.is("format3"));
  EXPECT(input.get<double>(0) == 3.2);
  EXPECT(input.get<string>(1) == "Text input.");
  EXPECT(input.get<double>("Option1", -1) == -1);
  EXPECT(input.get<string>("Option2", "Option2 value.") == "Option2 value.");
}

} // namespace

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
  RUN_TEST(testInputsSingleFormatOptionsDefault);
  RUN_TEST(testInputsSingleFormatOptionsUpdate);
  RUN_TEST(testInputsMultipleFormats);
}
