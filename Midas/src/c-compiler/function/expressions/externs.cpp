#include <iostream>
#include "function/expressions/shared/shared.h"
#include "function/expressions/shared/string.h"
#include "region/common/controlblock.h"
#include "region/common/heap.h"

#include "translatetype.h"

#include "function/expression.h"

Ref translateExternCall(
    GlobalState* globalState,
    FunctionState* functionState,
    BlockState* blockState,
    LLVMBuilderRef builder,
    ExternCall* call) {
  auto name = call->function->name->name;
  if (name == "__addIntInt") {
    assert(call->argExprs.size() == 2);
    auto leftLE =
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0]));
    auto rightLE =
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1]));
    auto result = LLVMBuildAdd(builder, leftLE, rightLE,"add");
    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, result);
  } else if (name == "__divideIntInt") {
    assert(call->argExprs.size() == 2);
    auto leftLE =
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0]));
    auto rightLE =
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1]));
    auto result = LLVMBuildSDiv(builder, leftLE, rightLE,"add");
    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, result);
//  } else if (name == "__eqStrStr") {
//    assert(call->argExprs.size() == 2);
//
//    auto leftStrTypeM = call->argTypes[0];
//    auto leftStrRef =
//        translateExpression(
//            globalState, functionState, blockState, builder, call->argExprs[0]);
//
//    auto rightStrTypeM = call->argTypes[1];
//    auto rightStrRef =
//        translateExpression(
//            globalState, functionState, blockState, builder, call->argExprs[1]);
//
//    std::vector<LLVMValueRef> argsLE = {
//        functionState->defaultRegion->getStringBytesPtr(functionState, builder, leftStrRef),
//        functionState->defaultRegion->getStringBytesPtr(functionState, builder, rightStrRef)
//    };
//    auto resultInt8LE =
//        LLVMBuildCall(
//            builder,
//            globalState->eqStr,
//            argsLE.data(),
//            argsLE.size(),
//            "eqStrResult");
//    auto resultBoolLE = LLVMBuildICmp(builder, LLVMIntNE, resultInt8LE, LLVMConstInt(LLVMInt8TypeInContext(globalState->context), 0, false), "");
//
//    functionState->defaultRegion->dealias(FL(), functionState, blockState, builder, globalState->metalCache.strRef, leftStrRef);
//    functionState->defaultRegion->dealias(FL(), functionState, blockState, builder, globalState->metalCache.strRef, rightStrRef);
//
//    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, resultBoolLE);
  } else if (name == "__strLength") {
    assert(call->argExprs.size() == 1);

    auto leftStrRef =
        translateExpression(
            globalState, functionState, blockState, builder, call->argExprs[0]);
    auto resultLenLE = functionState->defaultRegion->getStringLen(functionState, builder, leftStrRef);

    functionState->defaultRegion->dealias(
        FL(), functionState, blockState, builder, globalState->metalCache.strRef, leftStrRef);

    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, resultLenLE);
  } else if (name == "__addFloatFloat") {
    // VivemExterns.addFloatFloat
    assert(false);
  } else if (name == "__panic") {
    auto exitCodeLE = makeConstIntExpr(functionState, builder, LLVMInt8TypeInContext(globalState->context), 255);
    LLVMBuildCall(builder, globalState->exit, &exitCodeLE, 1, "");
    LLVMBuildRet(builder, LLVMGetUndef(functionState->returnTypeL));
    return wrap(functionState->defaultRegion, globalState->metalCache.neverRef, globalState->neverPtr);
  } else if (name == "__multiplyIntInt") {
    assert(call->argExprs.size() == 2);
    auto resultIntLE =
        LLVMBuildMul(
            builder,
            functionState->defaultRegion->checkValidReference(FL(),
                functionState, builder, call->function->params[0],
                translateExpression(
                    globalState, functionState, blockState, builder, call->argExprs[0])),
            functionState->defaultRegion->checkValidReference(FL(),
                functionState, builder, call->function->params[1],
                translateExpression(
                    globalState, functionState, blockState, builder, call->argExprs[1])),
            "mul");
    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, resultIntLE);
  } else if (name == "__subtractIntInt") {
    assert(call->argExprs.size() == 2);
    auto resultIntLE =
        LLVMBuildSub(
            builder,
            functionState->defaultRegion->checkValidReference(FL(),
                functionState, builder, call->function->params[0],
                translateExpression(
                    globalState, functionState, blockState, builder, call->argExprs[0])),
            functionState->defaultRegion->checkValidReference(FL(),
                functionState, builder, call->function->params[1],
                translateExpression(
                    globalState, functionState, blockState, builder, call->argExprs[1])),
            "diff");
    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, resultIntLE);
  } else if (name == "__getch") {
    auto resultIntLE = LLVMBuildCall(builder, globalState->getch, nullptr, 0, "");
    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, resultIntLE);
  } else if (name == "__lessThanInt") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildICmp(
        builder,
        LLVMIntSLT,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__greaterThanInt") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildICmp(
        builder,
        LLVMIntSGT,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__greaterThanOrEqInt") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildICmp(
        builder,
        LLVMIntSGE,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__lessThanOrEqInt") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildICmp(
        builder,
        LLVMIntSLE,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__eqIntInt") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildICmp(
        builder,
        LLVMIntEQ,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__eqBoolBool") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildICmp(
        builder,
        LLVMIntEQ,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__not") {
    assert(call->argExprs.size() == 1);
    auto result = LLVMBuildNot(
        builder,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__and") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildAnd(
        builder,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__or") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildOr(
        builder,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.boolRef, result);
  } else if (name == "__mod") {
    assert(call->argExprs.size() == 2);
    auto result = LLVMBuildSRem(
        builder,
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[0],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[0])),
        functionState->defaultRegion->checkValidReference(FL(),
            functionState, builder, call->function->params[1],
            translateExpression(
                globalState, functionState, blockState, builder, call->argExprs[1])),
        "");
    return wrap(functionState->defaultRegion, globalState->metalCache.intRef, result);
  } else {

    auto args = std::vector<Ref>{};
    args.reserve(call->argExprs.size());
    for (int i = 0; i < call->argExprs.size(); i++) {
      auto argExpr = call->argExprs[i];
      auto argRefMT = call->function->params[i];
      auto argRef = translateExpression(globalState, functionState, blockState, builder, argExpr);
      args.push_back(argRef);
    }

    auto argsLE = std::vector<LLVMValueRef>{};
    argsLE.reserve(call->argExprs.size());
    for (int i = 0; i < call->argExprs.size(); i++) {
      auto argRefMT = call->function->params[i];

      auto externalArgRefLE = functionState->defaultRegion->externalify(functionState, builder, argRefMT, args[i]);

      argsLE.push_back(externalArgRefLE);
    }

    auto externFuncIter = globalState->externFunctions.find(call->function->name->name);
    assert(externFuncIter != globalState->externFunctions.end());
    auto externFuncL = externFuncIter->second;

    buildFlare(FL(), globalState, functionState, builder, "Suspending function ", functionState->containingFuncName);
    buildFlare(FL(), globalState, functionState, builder, "Calling extern function ", call->function->name->name);

    for (int i = 0; i < call->argExprs.size(); i++) {
      auto argRefMT = call->function->params[i];
      // Dealias any object heading into the outside world, see DEPAR.
      functionState->defaultRegion->dealias(FL(), functionState, blockState, builder, argRefMT, args[i]);
    }

    auto resultLE = LLVMBuildCall(builder, externFuncL, argsLE.data(), argsLE.size(), "");
//    auto resultRef = wrap(functionState->defaultRegion, call->function->returnType, resultLE);
//    functionState->defaultRegion->checkValidReference(FL(), functionState, builder, call->function->returnType, resultRef);

    if (call->function->returnType->referend == globalState->metalCache.never) {
      buildFlare(FL(), globalState, functionState, builder, "Done calling function ", call->function->name->name);
      buildFlare(FL(), globalState, functionState, builder, "Resuming function ", functionState->containingFuncName);
      LLVMBuildRet(builder, LLVMGetUndef(functionState->returnTypeL));
      return wrap(functionState->defaultRegion, globalState->metalCache.neverRef, globalState->neverPtr);
    } else {
      buildFlare(FL(), globalState, functionState, builder, "Done calling function ", call->function->name->name);
      buildFlare(FL(), globalState, functionState, builder, "Resuming function ", functionState->containingFuncName);

      auto internalArgRef = functionState->defaultRegion->internalify(functionState, builder, call->function->returnType, resultLE);

      // Alias any object coming from the outside world, see DEPAR.
      functionState->defaultRegion->alias(FL(), functionState, builder, call->function->returnType, internalArgRef);

      return internalArgRef;
    }
  }
  assert(false);
}
