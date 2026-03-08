help([[
loads UFS Model prerequisites for Hercules/GNU
]])

prepend_path("MODULEPATH", "/apps/contrib/spack-stack/spack-stack-1.9.2/envs/ue-gcc-13.3.0/install/modulefiles/Core")
prepend_path("MODULEPATH", "/apps/contrib/spack-stack/modulefiles")

local stack_gnu_ver=os.getenv("stack_gnu_ver") or "13.3.0"
local stack_openmpi_ver=os.getenv("stack_openmpi_ver") or "4.1.6"
local cmake_ver=os.getenv("cmake_ver") or "3.27.9"
-- local nccmp_ver=os.getenv("nccmp_ver") or "1.9.0.1"

load(pathJoin("stack-gcc", stack_gnu_ver))
load(pathJoin("stack-openmpi", stack_openmpi_ver))
load(pathJoin("cmake", cmake_ver))
-- load(pathJoin("nccmp", nccmp_ver))

local modules = {
  {["jasper"]          = "2.0.32"},
  {["libpng"]          = "1.6.37"},
  {["hdf5"]            = "1.14.3"},
  {["netcdf-c"]        = "4.9.2"},
--  {["netcdf-fortran"]  = "4.6.1"},
--  {["parallelio"]      = "2.6.2"},
--  {["esmf"]            = "8.8.0"},
--  {["fms"]             = "2024.02"},
--  {["bacio"]           = "2.4.1"},
--  {["crtm"]            = "2.4.0.1"},
--  {["g2"]              = "3.5.1"},
--  {["g2tmpl"]          = "1.13.0"},
--  {["ip"]              = "5.1.0"},
--  {["sp"]              = "2.5.0"},
--  {["w3emc"]           = "2.10.0"},
  {["gftl-shared"]     = "1.9.0"},
  {["zlib"]            = "1.2.13"},
  {["qt"]              = "5.15.14"},
  {["ncl"]             = "6.6.2"},
}

for i = 1, #modules do
  for name, default_version in pairs(modules[i]) do
    local env_version_name = string.gsub(name, "-", "_") .. "_ver"
    load(pathJoin(name, os.getenv(env_version_name) or default_version))
  end
end

whatis("Description: StarViewer build environment")
