# 简介

本项目生成适用于 [**Shadowrocket**](https://www.shadowrocket.vip) 的规则集。

使用 **GitHub Actions** 北京时间每天**早上 7:30** 自动构建，保证规则最新。

## 说明

本项目规则集的数据主要来源于项目 [@Loyalsoldier/surge-rules](https://github.com/Loyalsoldier/surge-rules)。

## 规则文件地址及使用方式

### 在线地址（URL）

> 如果无法访问域名 `raw.githubusercontent.com`，可以使用第二个地址（`cdn.jsdelivr.net`）或第三个地址（`ghproxy.com`）

#### 规则文件:

- **白名单模式（GFWList代理 中国大陆域名和IP直连 其余代理）**：
  - [https://raw.githubusercontent.com/moesnow/Shadowrocket-Rules/release/rule.conf](https://raw.githubusercontent.com/moesnow/Shadowrocket-Rules/release/rule.conf)
  - [https://cdn.jsdelivr.net/gh/moesnow/Shadowrocket-Rules@release/rule.conf](https://cdn.jsdelivr.net/gh/moesnow/Shadowrocket-Rules@release/rule.conf)
  - [https://ghproxy.com/https://raw.githubusercontent.com/moesnow/Shadowrocket-Rules/release/rule.conf](https://ghproxy.com/https://raw.githubusercontent.com/moesnow/Shadowrocket-Rules/release/rule.conf)

### 使用方式

在”配置“选项内的“远程文件”中添加上述规则文件链接，然后点击链接选择“使用配置”。

## 致谢

- [@Loyalsoldier/surge-rules](https://github.com/Loyalsoldier/surge-rules)
- [@Loyalsoldier/geoip](https://github.com/Loyalsoldier/geoip)
- [@Loyalsoldier/v2ray-rules-dat](https://github.com/Loyalsoldier/v2ray-rules-dat)
- [@v2fly/domain-list-community](https://github.com/v2fly/domain-list-community)
- [@felixonmars/dnsmasq-china-list](https://github.com/felixonmars/dnsmasq-china-list)
- [@17mon/china_ip_list](https://github.com/17mon/china_ip_list)
