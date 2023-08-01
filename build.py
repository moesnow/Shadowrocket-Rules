import requests
from datetime import datetime

RULE_FILE_PATH = "rule.conf"

GFW_URL = "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/gfw.txt"
DIRECT_URL = "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/direct.txt"
CNCIDR_URL = "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/cncidr.txt"

HEADERS = '''[General]
# > 启用IPV6
ipv6 = true
prefer-ipv6 = true
bypass-system = true
# > 跳过代理
# 跳过某个域名或者 IP 段，这些目标主机将不会由 Surge Proxy 处理。
# (macOS 版本中，如果启用了 Set as System Proxy, 这些值会被写入到系统网络代理设置.)
skip-proxy = localhost, *.local, captive.apple.com, e.crashlytics.com, sequoia.apple.com, seed-sequoia.siri.apple.com, www.baidu.com, passenger.t3go.cn, yunbusiness.ccb.com, wxh.wo.cn, gate.lagou.com, www.abchina.com.cn, login-service.mobile-bank.psbc.com, mobile-bank.psbc.com, app.yinxiang.com, injections.adguard.org, local.adguard.org, cable.auth.com, *.id.ui.direct, 10.0.0.0/8, 100.64.0.0/10, 127.0.0.1/32, 169.254.0.0/16, 172.16.0.0/12, 192.168.0.0/16, 192.168.122.1/32, 193.168.0.1/32, 224.0.0.0/4, 240.0.0.0/4, 255.255.255.255/32, ::1/128, fc00::/7, fd00::/8, fe80::/10, ff00::/8, 2001::/32, 2001:db8::/32, 2002::/16, ::ffff:0:0:0:0/1, ::ffff:128:0:0:0/1
bypass-tun = 10.0.0.0/8,100.64.0.0/10,127.0.0.0/8,169.254.0.0/16,172.16.0.0/12,192.0.0.0/24,192.0.2.0/24,192.88.99.0/24,192.168.0.0/16,198.18.0.0/15,198.51.100.0/24,203.0.113.0/24,224.0.0.0/4,255.255.255.255/32
dns-server = https://doh.pub/dns-query, https://dns.alidns.com/dns-query
# > Always Real IP Hosts
# 当 Surge VIF 处理 DNS 问题时，此选项要求 Surge 返回一个真正的 IP 地址，而不是一个假 IP 地址。
# DNS 数据包将被转发到上游 DNS 服务器。
always-real-ip = %INSERT% *.lan, cable.auth.com, *.msftconnecttest.com, *.msftncsi.com, network-test.debian.org, detectportal.firefox.com, resolver1.opendns.com, *.srv.nintendo.net, *.stun.playstation.net, xbox.*.microsoft.com, *.xboxlive.com, stun.*, global.turn.twilio.com, global.stun.twilio.com, localhost.*.qq.com, localhost.*.weixin.qq.com, *.logon.battlenet.com.cn, *.logon.battle.net, *.blzstatic.cn, music.163.com, *.music.163.com, *.126.net, musicapi.taihe.com, music.taihe.com, songsearch.kugou.com, trackercdn.kugou.com, *.kuwo.cn, api-jooxtt.sanook.com, api.joox.com, joox.com, y.qq.com, *.y.qq.com, streamoc.music.tc.qq.com, mobileoc.music.tc.qq.com, isure.stream.qqmusic.qq.com, dl.stream.qqmusic.qq.com, aqqmusic.tc.qq.com, amobile.music.tc.qq.com, *.xiami.com, *.music.migu.cn, music.migu.cn, proxy.golang.org, *.mcdn.bilivideo.cn, *.cmpassport.com, id6.me, open.e.189.cn, mdn.open.wo.cn, opencloud.wostore.cn, auth.wosms.cn, *.jegotrip.com.cn, *.icitymobile.mobi, *.pingan.com.cn, *.cmbchina.com, pool.ntp.org, *.pool.ntp.org, ntp.*.com, time.*.com, ntp?.*.com, time?.*.com, time.*.gov, time.*.edu.cn, *.ntp.org.cn, PDC._msDCS.*.*, DC._msDCS.*.*, GC._msDCS.*.*
'''
TAIL = '''[URL Rewrite]
^https?://(www.)?(g|google)\.cn https://www.google.com 302

[MITM]
hostname = *.google.cn,*.googlevideo.com
'''


def generate_content_with_date():
    current_time = datetime.now()
    formatted_time = current_time.strftime("%Y-%m-%d %H:%M:%S")
    return f"# Generate date: {formatted_time}\n"


def process_url_content(url, domain_or_ip, rule):
    try:
        response = requests.get(url)
        response.raise_for_status()
        content = response.text.splitlines()

        result = []

        for line in content:
            if domain_or_ip.lower() == 'domain':
                if line.startswith('.'):
                    line = line[1:]
                line = f'DOMAIN-SUFFIX,{line}'
            line += f',{rule}'
            result.append(line)

        return '\n'.join(result) + '\n'

    except requests.exceptions.RequestException as e:
        print(f"请求URL时发生错误: {e}")
        return None


def main():
    Content = generate_content_with_date()
    Content += HEADERS + '\n'
    Content += "[Rule]\n"
    Content += process_url_content(GFW_URL, 'domain', 'PROXY')
    Content += process_url_content(DIRECT_URL, 'domain', 'DIRECT')
    Content += process_url_content(CNCIDR_URL, 'ip', 'DIRECT')
    Content += "FINAL,proxy\n"
    Content += '\n' + TAIL

    with open(RULE_FILE_PATH, 'w') as file:
        file.write(Content)

    print("内容已成功输出到文件中。")


if __name__ == "__main__":
    main()
