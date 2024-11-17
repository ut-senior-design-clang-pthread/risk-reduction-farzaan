//
// Created by Farza on 11/15/2024.
//
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"

using namespace clang;
using namespace ento;

namespace {

    class PthreadCreateChecker : public Checker<check::PreCall> {
    public:
        void checkPreCall(const CallEvent &Call, CheckerContext &C) const;
    };

    void PthreadCreateChecker::checkPreCall(const CallEvent &Call, CheckerContext &C) const {
        // Get the called function and cast it properly
        const FunctionDecl *FD = dyn_cast_or_null<FunctionDecl>(Call.getDecl());
        if (!FD) {
            return;
        }

        // Get the function name using getName()
        StringRef FName = FD->getName();

        // Check if the function name is pthread_create
        if (FName == "pthread_create") {
            // If so, report a bug
            BugType *BT = new BugType(this, "pthread_create called", categories::SecurityError);
            ExplodedNode *N = C.generateErrorNode();
            if (N) {
                auto R = std::make_unique<PathSensitiveBugReport>(*BT, "pthread_create function called", N);
                C.emitReport(std::move(R));
            }
        }
    }

} // end anonymous namespace

// Register the checker
void ento::registerPthreadCreateChecker(CheckerManager &mgr) {
    mgr.registerChecker<PthreadCreateChecker>();
}
bool ento::shouldRegisterPthreadCreateChecker(const CheckerManager &mgr) {
    return true;
}
