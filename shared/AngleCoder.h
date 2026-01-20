#pragma once
#include <algorithm>
#include <vector>

class AngleCoder
{
protected:
    const static int maxBytes = 1024;
    const static unsigned int startSequence = 'J' | 'K' << 8 | 'W' << 16 | 'A' << 24; // 4 bytes for identifier, then 4 bytes for size of message 
    const static unsigned int endSequence = 'T' | 'C' << 8 | 'H' << 16 | 'R' << 24; // 4 bytes for identifier, then 4 bytes for xor checksum

public:
    static inline float normalizeAngle(float angle)
    {
        //if (std::abs(angle % 1.0f) < 0.01f && angle < 0 && std::abs(angle) > 0.01f)
        if (std::abs(fmodf(angle, 1.0f)) < 0.01f && angle < 0 && std::abs(angle) > 0.01f)
        {
            // probably a badly snapped (int cast) negative number so we must actually do -1 here
            angle -= 1.0f;
        }
        //angle %= 360.0f;
        angle = fmodf(angle, 360.0f);
        if (angle < 0.0f)
        {
            angle += 360.0f;
        }
        return angle;
    }

};

struct Vector2_t {
    float X;
    float Y;
};

class AngleEncoder : AngleCoder {
public:
    //pitch,yaw
    static inline std::vector<Vector2_t> CreateAngleSequence(unsigned char* data, int len)
    {
        std::vector<Vector2_t> list;

        if (!data || !len) return list;

        unsigned int msgLen = (unsigned int)len;
        unsigned int xorNum = 0;
        for (int i = 0; i < msgLen; i++)
        {
            int shift2 = (i % 4) * 8;
            xorNum ^= ((unsigned int)data[i] << shift2);
        }

        Vector2_t newVec;
        // encode start sequence
        for (int i = 0; i < 4; i++)
        {
            unsigned char byteHere = (unsigned char)((startSequence & (255 << i * 8)) >> i * 8);
            newVec.X = i;
            newVec.Y = 255 + byteHere;
            list.push_back(newVec);
        }
        // encode message length as part of start sequence
        for (int i = 0; i < 4; i++)
        {
            unsigned char byteHere = (unsigned char)((msgLen & (255 << i * 8)) >> i * 8);
            newVec.X = i + 4;
            newVec.Y = byteHere;
            list.push_back(newVec);
        }


        for (int i = 0; i < msgLen; i++)
        {
            if (i % 8 == 0)
            {
                newVec.X = 45;
                newVec.Y = 45;
                list.push_back(newVec); // commit.
            }
            unsigned char byteHere = data[i];
            newVec.X = i % 8;
            newVec.Y = byteHere;
            list.push_back(newVec);
        }

        newVec.X = 45;
        newVec.Y = 45;
        list.push_back(newVec); // commit.


        // encode end sequence
        for (int i = 0; i < 4; i++)
        {
            unsigned char byteHere = (unsigned char)((endSequence & (255 << i * 8)) >> i * 8);
            newVec.X = i;
            newVec.Y = 255 + byteHere;
            list.push_back(newVec);
        }
        // encode xor checksum as part of start sequence
        for (int i = 0; i < 4; i++)
        {
            unsigned char byteHere = (unsigned char)((xorNum & (255 << i * 8)) >> i * 8);
            newVec.X = i + 4;
            newVec.Y = byteHere;
            list.push_back(newVec);
        }
        newVec.X = 45;
        newVec.Y = 45;
        list.push_back(newVec); // commit.

        std::vector<Vector2_t> adjustedList;
        for (int i = 0; i < list.size(); i++)
        {
            Vector2_t entry = list[i];
            entry.X += 0.25f;
            entry.Y += 0.25f;
            adjustedList.push_back(entry);
        }

        return adjustedList;
    }
};


class AngleDecoder : AngleCoder
{
    //object multiThreadLock = new object();
private:

    uint64_t buffer = 0;
    unsigned int specialBuffer = 0; // for start and end sequence. will be encoided as angles > 255.
    unsigned char fullBuffer[maxBytes] = { 0 };

    bool decodingActive = false;
    int decodingMessageSize = 0;
    int decodedChunks = 0;
    unsigned char receivedBits = 0;
    int nonsenseReceived = 0;

    inline void resetDecoding()
    {
        decodingActive = false;
        decodingMessageSize = 0;
        decodedChunks = 0;
        buffer = 0;
        specialBuffer = 0;
        memset(fullBuffer,0,sizeof(fullBuffer));
        receivedBits = 0;
        nonsenseReceived = 0;
    }

public:
    inline std::vector<unsigned char> GiveAngleMaybeReturnResult(float pitch, float yaw)
    {
        //lock(multiThreadLock) {

            pitch = normalizeAngle(pitch);
            yaw = normalizeAngle(yaw);

            //if (float.IsNaN(pitch) || float.IsNaN(yaw)) return null;
            if (pitch != pitch || yaw != yaw) return std::vector<unsigned char>();

            unsigned int pitchInt = (unsigned int)(pitch + 0.1f); // + 0.1 just in case rounded float numbers end up 0.99999
            unsigned int yawInt = (unsigned int)(yaw + 0.1f);
            int shift = 8 * (int)pitchInt;

            if (pitchInt >= 0 && pitchInt <= 7 && yawInt >= 0 && yawInt <= 255)
            {
                buffer &= ~((uint64_t)255U << shift);
                buffer |= (uint64_t)yawInt << shift;
                receivedBits |= (unsigned char)(1 << (int)pitchInt);
            }
            else if (pitchInt >= 0 && pitchInt <= 3 && yawInt >= 256 && yawInt <= 359)
            {
                specialBuffer &= ~(255U << shift);
                specialBuffer |= (yawInt - 255) << shift;
            }
            else if (pitchInt == 45 && yawInt == 45)
            {
                // This is the 'confirm chunk' angle combination
                if (decodingActive)
                {
                    if (specialBuffer == endSequence)
                    {
                        // Ok I guess we're done receiving the message.
                        // check length and xor
                        if (decodedChunks * 8 >= decodingMessageSize)
                        {
                            // good.
                            unsigned int xorNum = 0;
                            for (int i = 0; i < decodingMessageSize; i++)
                            {
                                int shift2 = (i % 4) * 8;
                                xorNum ^= ((unsigned int)fullBuffer[i] << shift2);
                            }

                            unsigned int control = (unsigned int)(buffer >> 32);

                            if (xorNum == control)
                            {
                                // successfully decoded
                                //unsigned char[] result = new byte[decodingMessageSize];
                                //Array.Copy(fullBuffer, result, decodingMessageSize);
                                std::vector<unsigned char> result(fullBuffer, fullBuffer+ decodingMessageSize);
                                resetDecoding();
                                return result;
                            }
                        }
                    }
                    else
                    {
                        if (decodedChunks * 8 < decodingMessageSize)
                        {
                            int bytesInThisChunk = std::min(8, decodingMessageSize - decodedChunks * 8);
                            unsigned char receivedBitsRequired = 0;
                            for (int i = 0; i < bytesInThisChunk; i++)
                            {
                                receivedBitsRequired |= (unsigned char)(1 << i);
                            }
                            if ((receivedBits & receivedBitsRequired) == receivedBitsRequired)
                            {
                                //unsigned char[] chunk = BitConverter.GetBytes(buffer);
                                //Array.Copy(chunk, 0, fullBuffer, decodedChunks * 8, 8);
                                const unsigned char* chunk = (const unsigned char*)&buffer;
                                memcpy(fullBuffer+decodedChunks*8,chunk,8);
                                decodedChunks++;
                                receivedBits = 0;
                            }
                        }
                    }
                }
                else
                {
                    if (specialBuffer == startSequence)
                    {
                        unsigned int messageLength = (unsigned int)(buffer >> 32);
                        if (messageLength <= maxBytes)
                        {
                            resetDecoding();
                            decodingMessageSize = (int)messageLength;
                            decodingActive = true;
                        }
                    }
                }
            }
            else if (decodingActive)
            {
                nonsenseReceived++;

                if (nonsenseReceived > 100)
                {
                    // ok it's some nonsense or too much noise
                    // reset everything
                    resetDecoding();
                }
            }

            return std::vector<unsigned char>();
        //}

    }
};
