#
# Copyright 2019 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Setup the tool
source /proj/xbuilds/2019.2_daily_latest/installs/lin64/Vitis/HEAD/settings64.sh 2>&1 1>/dev/null

# For NIMBIX test, stick to 2019.1 release
case `lsb_release -i` in
  *Ubuntu)
    source /proj/rdi/xbuilds/2019.1_daily_latest/xbb/xrt/packages/xrt-2.1.0-ubuntu1604/opt/xilinx/xrt/setup.sh 2>&1 1>/dev/null
    ;;
  *CentOS|*RedHat*)
    source /proj/rdi/xbuilds/2019.1_daily_latest/xbb/xrt/packages/xrt-2.1.0-centos/opt/xilinx/xrt/setup.sh 2>&1 1>/dev/null
    ;;
  *)
    echo OS not detected, XILINX_XRT not set!
    ;;
esac
export PLATFORM_REPO_PATHS=/proj/xbuilds/2019.1_daily_latest/xbb/dsadev/opt/xilinx/platforms

# Final checking
echo "XILINX_XRT: $XILINX_XRT"
echo "XILINX_VITIS: $XILINX_VITIS"
echo "XILINX_VIVADO: $XILINX_VIVADO"
echo "PLATFORM_REPO_PATHS: $PLATFORM_REPO_PATHS"
