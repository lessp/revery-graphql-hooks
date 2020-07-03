module type BaseConfig = {
  let baseUrl: string;
  let headers: list((string, string));
};
