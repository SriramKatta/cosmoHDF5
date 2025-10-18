# cosmoHDF5

This is a **proxy application** designed to benchmark and demonstrate **serial and parallel HDF5 I/O** operations for large-scale astrophysical simulation data — specifically targeting **IllustrisTNG-100** datasets.  
It provides a interface to efficiently **read and write cosmological snapshot data** using both single-process (serial) and distributed (MPI-parallel) workflows.

- Parallel I/O using **MPI + HDF5**  
- Compatibility with existing serial workflows  
- Modular design for integration into larger simulation pipelines  
- Benchmarking read/write scalability for real cosmological datasets  

## 📦 Dependencies

- **CMake ≥ 3.20**
- **MPI** (e.g., OpenMPI, MPICH)
- **HDF5** (compiled with parallel support)
- **C++17 or later**

---

## 🔧 Build Instructions

```bash
git clone https://github.com/SriramKatta/cosmoHDF5.git
cd cosmoHDF5
cmake -S. -Bbuild
cmake --build build -j
