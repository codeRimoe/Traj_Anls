# Traj_Anls
Cellular signaling data analysis and visualization, implement by C++ with QT.

For Windows User, just download the file [Traj_Anls_x86_v1.50b_release.zip](https://github.com/codeRimoe/Traj_Anls/Traj_Anls_x86_v1.50b_release.zip), you can use directly after unzipping.
The user guide [Traj_Anls_v1.50b_user_guide.pdf](https://github.com/codeRimoe/Traj_Anls/Traj_Anls_v1.50b_user_guide.pdf) may help you start to use this software, but note that the document is in Chinese.

About the Home/Work Points Detection
The Home/Work Points are detected based on the criteria below.
> 1.筛选四个停驻点ABCD：A全天停驻时间最多，B晚上停驻时间最多，C白天停驻时间最多，D白天停驻时间第二多（或null），ABCD有可能有2/3个点相同，但CD必不相同。

> 2.若B点晚上停驻时间大于家的阈值，那么B点识别为家，否则A点识别为家。

> 3.若C点白天停驻时间大于等于公司阈值C点在2中未被识别为家，那么C点识别为公司，否则进入第4步。

> 4.若D点白天停驻时间大于等于公司阈值，则D点识别为公司，否则认为该用户无固定办公地或无业。

---

# License
```
                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   Authorright 2018 Yue H.W. Luo, yue.rimoe@gmail.com .
   Copyright 2018 Sun Yat-sen University .

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
```