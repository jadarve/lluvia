
import lluvia as ll
import src

def main():
    print('moni moni')
    print(ll.__dict__)
    print(src.__path__)
    
    ll.Parameter(value=2)
    

if __name__ == "__main__":
    main()
