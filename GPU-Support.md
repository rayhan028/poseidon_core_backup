
## GPU-based graph analytics leveraging Gunrock

In order to use Gunrock to run graph analytics within Poseidon (given the current prototypical implementation), you need to perform several initial steps. 

* Download Gunrock and install all necessary dependencies. You can find details on their website: https://gunrock.github.io/gunrock/doc/latest/building_gunrock.html

* Guide for installing CUDA: https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html#abstract. Do not forget to follow the "Post-installation Actions"!

* Since Gunrock is still under development and not everything just works out-of-the-box, you will need to add some API-functions to Gunrock. We will be use them to hand over our graph in either COO or CSR graph representation format. Go to the files `{poseidon_dir}/gunrock/gunrock-h_changes.txt` and `{poseidon_dir}/gunrock/sssp_acc-cu_changes.txt` and copy the code you find there to the end of the respective file `{gunrock_dir}/gunrock/gunrock.h` and `{gunrock_dir}/gunrock/app/sssp/sssp_app.cu` within Gunrock. This procedure is a bit bulky and might be improved upon in the future, if possible

* Compile Gunrock (this takes a while). When finished, copy the file `{gunrock_dir}/gunrock/gunrock.h` to `{poseidon_dir}/src/analytics` and `{gunrock_dir}/build/lib/libgunrock.so` to `/usr/lib` for the linker to find it

You can now build the Poseidon project by using 
```
cmake -DUSE_GUNROCK=ON .. 
```
or switching the flag `USE_GUNROCK` in `{poseidon_dir}/CMakeLists.txt` to `ON`. You should now be able to use the provided weighted SSSP implementations. 