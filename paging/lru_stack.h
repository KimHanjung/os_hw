class l_stack
{
    private:
        int index;
        int stacking[32];
        void back(int pos)
        {
            for(int i=pos; i<index-1; i++)
                stacking[i] = stacking[i+1];
        }
        bool empty()
        {
            return (index==0);
        }

    public:
        l_stack()
        {
            index = 0;
        }
        void push(int data)
        {
            bool check = false;
            int pos;
            for(int i=0; i<index; i++)
            {
                if(stacking[i]==data) 
                {
                    check = true;
                    pos = i;
                    break;
                }
            }

            if(check)
            {
                back(pos);
                stacking[index-1] = data;
            }
            else
                stacking[index++] = data;
        }
        int pop()
        {
            if(empty()) return 0;
            int result = stacking[0];
            back(0);
            index--;
            return result;
        }
        void pop(int data)
        { 
            if(!empty())
            {
                int pos;
                for(int i=0; i<index; i++)
                {
                    if(stacking[i]==data) 
                    {
                        pos = i;
                        break;
                    }
                }
                back(pos);
                index--;
            }
        }
};