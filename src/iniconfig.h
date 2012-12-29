/*
 * File       : iniconfig.h
 * Description: Ini配置文件
 * Version    : 2010-10-16 Created
 *              2011-09-24 1.1 多操作系统支持
 *              2011-12-16 1.2 增加环境变量解析
 * Author     : buf1024@gmail.com
 */

#ifndef __48SLOTS_INICONFIG_H__
#define __48SLOTS_INICONFIG_H__

#include <list>
#include <map>
#include <string>

class Section
{
    typedef std::map< std::string, std::string >::iterator KVIterator;
public:
    Section(std::string strSectionName = "");
    ~Section();
public:
    void SetSectionName(const std::string strSectionName);
    std::string GetSectionName() const;
    void Insert(const std::string strKey, std::string strValue);
    void Insert(const std::string strKey, char chValue);
    void Insert(const std::string strKey, int nValue);
    void Insert(const std::string strKey, long lValue);
    void Insert(const std::string strKey, double dValue);
    bool GetValue(const std::string strKey, std::string& strValue, std::string strDefault = "");
    bool GetValue(const std::string strKey, bool& bValue, bool bDefault = false);
    bool GetValue(const std::string strKey, int& nValue, int nDefault = 0);
    bool GetValue(const std::string strKey, long& lValue, long lDefault = 0L);
    bool GetValue(const std::string strKey, double& dValue, double fDefault = 0.0f);
    void Delete(const std::string strKey);
    void Empty();
    bool Save(std::string strFilePath);
    bool Save(FILE* pFile);

private:
    std::string m_strSectionName;
    std::map< std::string, std::string > m_mapKeyValue;

};

class IniConfig
{
    typedef std::list<Section*>::iterator ICIterator;
public:
    typedef std::list<Section*>::const_iterator Iterator;
public:
    IniConfig(std::string strFilePath = "");
    ~IniConfig();
    void SetFilePath(const std::string strFilePath);
    std::string GetFilePath() const;

    bool Insert(Section* pSec);
    void Delete(const std::string strSection);

    Section* GetSection(const std::string strSecName);

    bool Load(const std::string strFilePath = "");
    bool Save(const std::string strFilePath = "");

    Iterator begin() const
    {
        return m_lstSections.begin();
    }
    Iterator end() const
    {
        return  m_lstSections.end();
    }
private:
    void ClearUp();
private:
    std::string m_strFilePath;
    std::list<Section*> m_lstSections;

};


#endif /* __48SLOTS_INICONFIG_H__ */
