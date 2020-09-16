#pragma once
#include<algorithm>
#include<string>
#include<utility>
#include<vector>

namespace utils{
    inline std::string tolower(std::string str){
	    std::transform(str.begin(), str.end(),str.begin(),::tolower);
	    return str;
	}

	inline std::string toupper(std::string str){
	    std::transform(str.begin(), str.end(),str.begin(),::toupper);
	    return str;
	}

	inline std::string toregex(std::string src) {
		std::string dst;
		for (int i = 0, len = src.size(); i < len; i++) {
			if (src[i] == '%') {
				dst += ".*?";
			}
			else dst.push_back(src[i]);
		}
		return dst;
	}
	//去除首尾空格
    inline void trim(std::string& str) {
        if(!str.empty()){
            str.erase(0, str.find_first_not_of(" "));
            str.erase(str.find_last_not_of(" ") + 1);
        }
    }
    //分割并去除首尾空格
    inline void split(const std::string& s, std::vector<std::string>& v, const std::string& c) {
        std::string::size_type pos1, pos2;
        pos2 = s.find(c);
        pos1 = 0;
        while(std::string::npos != pos2) {
            v.push_back(s.substr(pos1, pos2-pos1));

            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if(pos1 != s.length())
            v.push_back(s.substr(pos1));
        for(int i=0;i<v.size();i++)
            trim(v[i]);
    }
    inline void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst) {
        std::string::size_type pos = 0;
        std::string::size_type srclen = strsrc.size();
        std::string::size_type dstlen = strdst.size();

        while( (pos=strBig.find(strsrc, pos)) != std::string::npos ) {
            strBig.replace( pos, srclen, strdst );
            pos += dstlen;
        }
    }
    inline char getlast_notblank(std::string& s){
        for(int i=s.length()-1;i>=0;i--){
            if(s[i] != ' ')return s[i];
        }
        return 0;
    }
}
