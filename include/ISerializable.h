#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H


class ISerializable
{
    public:
        ISerializable() = delete;
        virtual ~ISerializable();
        void virtual Serialize() = 0;
        void virtual Deserialize() = 0;
    protected:

    private:
};

#endif // ISERIALIZABLE_H
