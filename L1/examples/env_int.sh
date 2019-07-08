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

case `lsb_release -i` in
  *Ubuntu)
    export XILINX_XRT=/proj/rdi/xbuilds/2018.3_daily_latest/xbb/xrt/packages/xrt-2.1.0-ubuntu1604/opt/xilinx/xrt/
    ;;
  *CentOS|*RedHat*)
    export XILINX_XRT=/proj/rdi/xbuilds/2018.3_daily_latest/xbb/xrt/packages/xrt-2.1.0-centos/opt/xilinx/xrt
    ;;
  *)
    echo OS not detected, XILINX_XRT not set!
    ;;
esac
export XILINX_SDX=/proj/xbuilds/2018.3_daily_latest/installs/lin64/SDx/HEAD
export XILINX_VIVADO=/proj/xbuilds/2018.3_daily_latest/installs/lin64/Vivado/HEAD

export PLATFORM_REPO_PATHS=/proj/xbuilds/2018.3_daily_latest/xbb/dsadev/opt/xilinx/platforms
echo "PLATFORM_REPO_PATHS: $PLATFORM_REPO_PATHS"

export DEVICE=u200_xdma_201830_1
echo "DEVICE: $DEVICE"
