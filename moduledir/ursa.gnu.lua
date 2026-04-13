help([[
loads UFS Model prerequisites for Hercules/GNU
]])

prepend_path("MODULEPATH", "/contrib/spack-stack/spack-stack-1.9.2/envs/ue-gcc-12.4.0/install/modulefiles/Core")

local stack_gnu_ver=os.getenv("stack_gnu_ver") or "12.4.0"
-- local stack_openmpi_ver=os.getenv("stack_openmpi_ver") or "4.1.6"
-- local cmake_ver=os.getenv("cmake_ver") or "3.27.9"

load(pathJoin("stack-gcc", stack_gnu_ver))
-- load(pathJoin("stack-openmpi", stack_openmpi_ver))
-- load(pathJoin("cmake", cmake_ver))

local modules = {
-- {["meson"]           = "1.5.1"},
-- {["qt"]              = "5.15.14"},
  {["ninja"]           = "1.12.1"},
  {["jasper"]          = "2.0.32"},
  {["libpng"]          = "1.6.37"},
  {["hdf5"]            = "1.14.3"},
  {["netcdf-c"]        = "4.9.2"},
  {["gftl-shared"]     = "1.9.0"},
  {["zlib"]            = "1.2.13"},
-- {["ncl"]             = "6.6.2"},
  {["cmake"]           = "3.30.2"},
  {["cuda"]            = "13.1.0"},
}

for i = 1, #modules do
  for name, default_version in pairs(modules[i]) do
    local env_version_name = string.gsub(name, "-", "_") .. "_ver"
    load(pathJoin(name, os.getenv(env_version_name) or default_version))
  end
end

whatis("Description: StarViewer build environment")
