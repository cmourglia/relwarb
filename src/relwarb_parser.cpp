
#include <fstream>
#include <string>

#include "relwarb_parser.h"
#include "relwarb_debug.h"
#include "relwarb_defines.h"
#include "relwarb_entity.h"

#define SEPARATOR " \t"

enum ObjectParsing
{
    ObjectParsing_Unknown = -1,
    ObjectParsing_Bitmap = 0,
    ObjectParsing_Shape,
    ObjectParsing_RenderingPattern,
    ObjectParsing_Entity,
    ObjectParsing_ShapeToEntity,
    ObjectParsing_PatternToEntity
};

int SwitchParseObject(const std::string & header)
{
    if (header == "Bitmap")
        return ObjectParsing_Bitmap;
    if (header == "Shape")
        return ObjectParsing_Shape;
    if (header == "Pattern")
        return ObjectParsing_RenderingPattern;
    if (header == "Entity")
        return ObjectParsing_Entity;
    if (header == "Sh2En")
        return ObjectParsing_ShapeToEntity;
    if (header == "Pa2En")
        return ObjectParsing_PatternToEntity;

    Log(Log_Error, "Wrong header '%s'", header);
    return ObjectParsing_Unknown;
}

void ExtractString(std::string& line, std::string& extract)
{
    size_t separator = line.find_first_of(SEPARATOR);
    extract = line.substr(0, separator);

    if (separator != std::string::npos)
        line = line.substr(separator + 1);
    else
        line = "";
}

void ExtractVec2(std::string& line, z::vec2& extract)
{
    std::string token;
    size_t separator = line.find_first_of(SEPARATOR);
    token = line.substr(0, separator);
    real32 x = std::stof(token);
    line = line.substr(separator + 1);
    separator = line.find_first_of(SEPARATOR);
    token = line.substr(0, separator);
    real32 y = std::stof(token);
    extract = z::vec2(x, y);

    if (separator != std::string::npos)
        line = line.substr(separator + 1);
    else
        line = "";
}

void ExtractUint8(std::string& line, uint8& extract)
{
    std::string token;
    size_t separator = line.find_first_of(SEPARATOR);
    token = line.substr(0, separator);
    extract = std::stoi(token);

    if (separator != std::string::npos)
        line = line.substr(separator + 1);
    else
        line = "";
}

bool LoadMapFile(GameState * gameState, char * mapfile)
{
    std::ifstream ini;
    ini.open(mapfile);

    if (ini.is_open())
    {
        std::string previousLocale = setlocale(LC_NUMERIC, "en-US");
        std::string currentLine;

        while (getline(ini, currentLine))
        {
            if (!currentLine.empty() && !(currentLine[0] == '#'))
            {
                std::string header;
                ExtractString(currentLine, header);

                switch (SwitchParseObject(header))
                {
                    case ObjectParsing_Bitmap:
                    {
                        std::string filePath;
                        ExtractString(currentLine, filePath);
                        Bitmap * bitmap = CreateBitmap(gameState);
                        LoadBitmapData(filePath.c_str(), bitmap);
                        break;
                    }
                    case ObjectParsing_Shape:
                    {
                        z::vec2 size;
                        ExtractVec2(currentLine, size);
                        z::vec2 offset(0.f);
                        if (!currentLine.empty())
                        {
                            ExtractVec2(currentLine, offset);
                        }
                        CreateShape(gameState, size, offset);
                        break;
                    }
                    case ObjectParsing_RenderingPattern:
                    {
                        z::vec2 size;
                        ExtractVec2(currentLine, size);
                        uint8* pattern = new uint8[size.x()*size.y()];
                        for (uint8 idx = 0; idx < size.x()*size.y(); ++idx)
                        {
                            ExtractUint8(currentLine, pattern[idx]);
                        }
                        uint8 nbBitmaps;
                        ExtractUint8(currentLine, nbBitmaps);
                        Bitmap** bitmaps = new Bitmap*[nbBitmaps];
                        for (uint8 idx = 0; idx < nbBitmaps; ++idx)
                        {
                            uint8 bitmapIdx;
                            ExtractUint8(currentLine, bitmapIdx);
                            bitmaps[idx] = &(gameState->bitmaps[bitmapIdx - 1]);
                        }
                        uint8 type = RenderingPattern_Unique;
                        if (!currentLine.empty())
                        {
                            ExtractUint8(currentLine, type);
                        }
                        // TODO(Thomas): Handle non fill pattern loading
                        //               Pass the type of the pattern first
                        CreateFillRenderingPattern(gameState, size, pattern, nbBitmaps, bitmaps);

                        delete pattern;
                        delete bitmaps;
                        break;
                    }
                    case ObjectParsing_Entity:
                    {
                        uint8 type;
                        ExtractUint8(currentLine, type);
                        z::vec2 p, dp(0), ddp(0);
                        ExtractVec2(currentLine, p);
                        if (!currentLine.empty())
                        {
                            ExtractVec2(currentLine, dp);
                        }
                        if (!currentLine.empty())
                        {
                            ExtractVec2(currentLine, ddp);
                        }
                        CreateEntity(gameState, EntityType(type), p, dp, ddp);
                        break;
                    }
                    case ObjectParsing_ShapeToEntity:
                    {
                        uint8 entity, shape;
                        ExtractUint8(currentLine, entity);
                        ExtractUint8(currentLine, shape);
                        AddShapeToEntity(&(gameState->entities[entity - 1]), &(gameState->shapes[shape - 1]));
                        break;
                    }
                    case ObjectParsing_PatternToEntity:
                    {
                        uint8 entity, pattern;
                        ExtractUint8(currentLine, entity);
                        ExtractUint8(currentLine, pattern);
                        AddRenderingPatternToEntity(&(gameState->entities[entity - 1]), &(gameState->patterns[pattern - 1]));
                        break;
                    }
                    default:
                        return false;
                }
            }
        }

        ini.close();
        setlocale(LC_NUMERIC, previousLocale.c_str());
        return true;
    }
    else
    {
        Log(Log_Error, "Could not open map file '%s'", mapfile);
        return false;
    }
}
