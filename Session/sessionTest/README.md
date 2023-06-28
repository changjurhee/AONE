# tlsSession

* vcpkg 설치 : https://cheongpark.tistory.com/15
* openssl install : .\vcpkg install openssl:x64-windows
* visual studio linker
  
> Windows에서 소켓을 사용하기 위해 ws2_32.lib 라이브러리를 링크해야 합니다.
> Visual Studio에서 프로젝트 설정을 변경하여 ws2_32.lib를 추가하는 방법은 다음과 같습니다:
1. Visual Studio에서 프로젝트를 엽니다.
2. 솔루션 탐색기에서 프로젝트를 마우스 오른쪽 버튼으로 클릭하고 "속성"을 선택합니다.
3. "속성" 창에서 "구성 속성"을 확장하고 "링커"를 선택합니다.
4. "입력" 항목을 선택합니다.
5. "추가 종속성" 속성을 편집하고, ws2_32.lib를 추가합니다.
6. 변경 사항을 저장하고 프로젝트를 다시 빌드합니다.
위의 단계를 따라 ws2_32.lib를 추가하면 소켓 관련 함수들을 올바르게 링크하여 오류가 해결될 것입니다.

> 개인키 생성
```
openssl genpkey -algorithm RSA -out private.key
```
> 인증서 생성
```
openssl req -new -key private.key -out csr.csr
```
> 자체 서명 인증서 생성
```
openssl req -x509 -sha256 -days 365 -key private.key -in csr.csr -out certificate.crt
```
