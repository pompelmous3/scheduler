#include "../headers/tool.h"

std::string centerText(const std::string& text, int length)
{
    int textLength = text.length();

    if (textLength > length) {
        return text;
    }

    int padding = (length - textLength) / 2;
    std::string centeredText;
    centeredText.append(padding, ' ');
    centeredText.append(text);
    centeredText.append(length - textLength - padding, ' ');
    return centeredText;
}

std::string specialWrapCenterText(const std::string& text, int length, char chr)
{
    std::string res = centerText(text, length);
    res.erase(0, 1);
    res.insert(res.begin(), chr);
    res.erase(res.length()-1);
    res.push_back(chr);
    return res;
}

std::string ljust(const std::string& text, int length)
{
    int textLength = text.length();
    if (textLength > length) {
        return text;
    }
    std::string rest = std::string(length - textLength, ' ');

    return text+rest;
}

std::string specialLJust(const std::string& text, int length, char ch)
{
    std::string ret = std::string("#");
    ret.append(ljust(text, length-2));
    ret.append("#");
    return ret;
}