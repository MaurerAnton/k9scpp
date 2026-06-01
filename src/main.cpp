// k9scpp - C++ Kubernetes CLI explorer (port of k9s)
// Uses system kubectl commands, ANSI terminal rendering

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>

static const char* VERSION = "0.1.0";
static const char *R="\033[0m",*B="\033[1m",*D="\033[2m",*C="\033[36m",*G="\033[32m",*Y="\033[33m",*RD="\033[31m",*M="\033[35m";

static std::string kubectl(const std::string& args) {
    std::string cmd = "kubectl " + args + " 2>&1";
    FILE* f = popen(cmd.c_str(), "r");
    if (!f) return ""; char buf[4096]; std::string o;
    while (fgets(buf,sizeof(buf),f)) o += buf;
    pclose(f); return o;
}

struct Pod { std::string name, ready, status, restarts, age, node; };
struct Svc { std::string name, type, clusterIP, ports, age; };
struct Deploy { std::string name, ready, upToDate, available, age; };
struct NS { std::string name, status, age; };

static std::vector<Pod> getPods(const std::string& ns) {
    std::vector<Pod> pods;
    std::string out = kubectl("get pods -n " + ns + " --no-headers 2>/dev/null");
    std::istringstream ss(out); std::string line;
    while (std::getline(ss,line)) {
        std::istringstream ls(line); Pod p;
        ls >> p.name >> p.ready >> p.status >> p.restarts >> p.age;
        /* remaining is node */
        std::string rest; std::getline(ls, rest);
        p.node = rest; while (!p.node.empty() && p.node[0]==' ') p.node = p.node.substr(1);
        if (!p.name.empty()) pods.push_back(p);
    }
    return pods;
}

static std::vector<Svc> getSvcs(const std::string& ns) {
    std::vector<Svc> svcs;
    std::string out = kubectl("get svc -n " + ns + " --no-headers 2>/dev/null");
    std::istringstream ss(out); std::string line;
    while (std::getline(ss,line)) {
        std::istringstream ls(line); Svc s;
        ls >> s.name >> s.type >> s.clusterIP >> s.ports >> s.age;
        if (!s.name.empty()) svcs.push_back(s);
    }
    return svcs;
}

static std::vector<Deploy> getDeploys(const std::string& ns) {
    std::vector<Deploy> deploys;
    std::string out = kubectl("get deploy -n " + ns + " --no-headers 2>/dev/null");
    std::istringstream ss(out); std::string line;
    while (std::getline(ss,line)) {
        std::istringstream ls(line); Deploy d;
        ls >> d.name >> d.ready >> d.upToDate >> d.available >> d.age;
        if (!d.name.empty()) deploys.push_back(d);
    }
    return deploys;
}

static std::vector<NS> getNamespaces() {
    std::vector<NS> nss;
    std::string out = kubectl("get ns --no-headers 2>/dev/null");
    std::istringstream ss(out); std::string line;
    while (std::getline(ss,line)) {
        std::istringstream ls(line); NS n;
        ls >> n.name >> n.status >> n.age;
        if (!n.name.empty()) nss.push_back(n);
    }
    return nss;
}

int main(int argc, char* argv[]) {
    std::string ns = "default";
    bool showPods = true, showSvcs = false, showDeploys = false, showNs = false;

    for (int i=1;i<argc;i++) {
        std::string a=argv[i];
        if (a=="-n" && i+1<argc) ns = argv[++i];
        else if (a=="--pods") showPods=true;
        else if (a=="--svc") showSvcs=true;
        else if (a=="--deploy") showDeploys=true;
        else if (a=="--ns") showNs=true;
        else if (a=="-A" || a=="--all-namespaces") ns="--all-namespaces";
    }

    printf("%s%s k9scpp %s%s%s  namespace: %s%s%s\n\n", B, C, VERSION, R, D, Y, ns.c_str(), D, R);

    if (showNs) {
        auto nss = getNamespaces();
        printf("%sNAMESPACE    STATUS   AGE%s\n", B, R);
        for (auto& n : nss) printf("%-12s %-7s %s\n", n.name.c_str(), n.status.c_str(), n.age.c_str());
        printf("\n");
    }

    if (showPods || (!showSvcs && !showDeploys && !showNs)) {
        auto pods = getPods(ns);
        printf("%sPODS (%zu)%s\n", B, pods.size(), R);
        printf("%-40s %-8s %-12s %-8s %-8s %s\n", "NAME","READY","STATUS","RESTARTS","AGE","NODE");
        printf("%s──%s\n", D, R);
        for (auto& p : pods) {
            const char* color = (p.status=="Running")?G:(p.status=="Pending")?Y:RD;
            printf("%s%-40s %-8s %-12s %-8s %-8s %-20s%s\n", color, p.name.c_str(), p.ready.c_str(),
                   p.status.c_str(), p.restarts.c_str(), p.age.c_str(), p.node.c_str(), R);
        }
        printf("\n");
    }

    if (showSvcs) {
        auto svcs = getSvcs(ns);
        printf("%sSERVICES (%zu)%s\n", B, svcs.size(), R);
        printf("%-40s %-12s %-16s %-12s %s\n","NAME","TYPE","CLUSTER-IP","PORTS","AGE");
        printf("%s──%s\n",D,R);
        for (auto& s : svcs) printf("%-40s %-12s %-16s %-12s %s\n", s.name.c_str(), s.type.c_str(), s.clusterIP.c_str(), s.ports.c_str(), s.age.c_str());
        printf("\n");
    }

    if (showDeploys) {
        auto deps = getDeploys(ns);
        printf("%sDEPLOYMENTS (%zu)%s\n", B, deps.size(), R);
        printf("%-40s %-12s %-12s %-12s %s\n","NAME","READY","UP-TO-DATE","AVAILABLE","AGE");
        printf("%s──%s\n",D,R);
        for (auto& d : deps) printf("%-40s %-12s %-12s %-12s %s\n", d.name.c_str(), d.ready.c_str(), d.upToDate.c_str(), d.available.c_str(), d.age.c_str());
        printf("\n");
    }

    return 0;
}
