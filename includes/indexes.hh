#pragma once

#include <vector>

#include <llvm/ADT/APInt.h>
#include <llvm/Constants.h>
#include <llvm/LLVMContext.h>
#include <llvm/Value.h>

/*
 * Definition of the function which turns a user GEP index into a LLVM value
 */

namespace mpllvm
{
    namespace internal
    {

        llvm::Value * getLLVMValueFromIndex( llvm::Value * t ) {
            return t;
        }

        llvm::Value * getLLVMValueFromIndex( llvm::LLVMContext & llvmContext, std::int32_t value ) {
            return llvm::ConstantInt::get( llvmContext, llvm::APInt( 32, value ) );
        }

        llvm::Value * getLLVMValueFromIndex( llvm::LLVMContext & llvmContext, std::int64_t value ) {
            return llvm::ConstantInt::get( llvmContext, llvm::APInt( 64, value ) );
        }

    }
}

/*
 * Definition of the IndexesResolver
 * (Which should convert multiple indexes into a std::vector<llvm::Value *>)
 */

namespace mpllvm
{

    namespace internal
    {

        template < typename... Indexes >
        struct IndexesResolver;

        template < typename T, typename... Rest >
        struct IndexesResolver< T, Rest... > {
            static std::vector< llvm::Value * > list( int position, llvm::LLVMContext & llvmContext, T t, Rest... rest ) {
                std::vector< llvm::Value * > && indexList = mpllvm::internal::IndexesResolver< Rest... >::list( position + 1, llvmContext, rest... );
                indexList[ position ] = mpllvm::internal::getLLVMValueFromIndex( llvmContext, t );
                return indexList;
            }
        };

        template < >
        struct IndexesResolver< > {
            static std::vector< llvm::Value * > list( int count, llvm::LLVMContext & ) {
                std::vector< llvm::Value * > indexList;
                indexList.resize( count );
                return indexList;
            }
        };

    }

}

/*
 * And here comes the tools providen by the library
 */

namespace mpllvm
{

    template < typename... Indexes >
    struct GEP {

        template < typename IRBuilder >
        static llvm::Value * build( llvm::LLVMContext & llvmContext, IRBuilder & builder, llvm::Value * value, Indexes... indexes ) {
            return builder.CreateGEP( value, mpllvm::internal::IndexesResolver< Indexes... >::list( 0, llvmContext, indexes... ) );
        }

    };

}
