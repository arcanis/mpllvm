#pragma once

#include <vector>

#include <llvm/ADT/APInt.h>
#include <llvm/Constants.h>
#include <llvm/LLVMContext.h>
#include <llvm/Value.h>

/*
 * Definition of the IndexesResolver
 * (Which should convert multiple indexes into a std::vector<llvm::Value *>)
 */

namespace mpllvm
{

    namespace internal
    {

        template < int... Indexes >
        struct IndexesResolver;

        template < int BitSize, int Offset, int... Rest >
        struct IndexesResolver< BitSize, Offset, Rest... > {
            static std::vector< llvm::Value * > list( llvm::LLVMContext & llvmContext, int position = 0 ) {
                std::vector< llvm::Value * > && indexList = mpllvm::internal::IndexesResolver< Rest... >::list( llvmContext, position + 1 );
                indexList[ position ] = llvm::ConstantInt::get( llvmContext, llvm::APInt( BitSize, Offset ) );
                return indexList;
            }
        };

        template < >
        struct IndexesResolver< > {
            static std::vector< llvm::Value * > list( llvm::LLVMContext &, int count = 0 ) {
                std::vector< llvm::Value *> indexList;
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

    template < int... N >
    struct gep {
        template < typename IRBuilder >
        static llvm::Value * build( llvm::LLVMContext & llvmContext, IRBuilder & builder, llvm::Value * value ) {
            return builder.CreateGEP( value, mpllvm::internal::IndexesResolver< N... >::list( llvmContext ) );
        }
    };

}
