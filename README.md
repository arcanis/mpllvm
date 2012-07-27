trivia.mpllvm
=============

Metaprogramming type resolver for LLVM (using C++11)

- `llvm::Type * mpllvm::get< T >( llvm::LLVMContext & llvmContext )`

  Returns the corresponding LLVM type of `T`.

      llvm::Type * foo = mpllvm::get< int >( llvmContext );

- `llvm::Type * mpllvm::deduce( llvm::LLVMContext & llvmContext, T const & t )`

  Returns the corresponding LLVM type of the value `t`.

      llvm::Type * foo = mpllvm::deduce( llvmContext, &malloc );

- `llvm::StructType * mpllvm::craft< T... >( llvm::LLVMContext & llvmContext, bool isPacked = false )`

  Returns a LLVM structure type whose each field is one of the template parameter.

      llvm::StructType * foo = mpllvm::craft< int, int >( llvmContext );
